#include "molotov.hpp"

#include "../../cache/cache.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <deps/delaunator-cpp/delaunator.hpp>
#include <imgui_internal.h>

static constexpr float orient(const ImVec2& a, const ImVec2& b, const ImVec2& c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// returns list of pair when outer or not
std::optional<std::vector<std::pair<bool, ImVec2>>> grahamScanIndicies(std::span<const ImVec2> points)
{
	// case when it's impossible
	if (points.size() < 3)
		return std::nullopt;

	// make a temp copy to allow use swap
	std::vector<ImVec2> v{ points.begin(), points.end() };

	// now at index 0 we have most left point
	std::ranges::iter_swap(v.begin(), std::ranges::min_element(v, {}, [](const ImVec2& p) { return p.x; }));

	std::ranges::sort(v.begin() + 1, v.end(),
		[p0 = v.at(0)](const ImVec2& a, const ImVec2& b)
		{
			auto _orient = orient(p0, a, b);
			return _orient == 0.0f ? ImLengthSqr(p0 - a) < ImLengthSqr(p0 - b) : _orient < 0.0f;
		}
	);

	std::vector<std::pair<size_t, ImVec2>> hull;
	for (size_t i = 0; const auto& el : v)
	{
		while (hull.size() > 1 && orient(hull[hull.size() - 2].second, hull[hull.size() - 1].second, el) >= 0.0f)
			hull.pop_back();

		hull.emplace_back(std::make_pair(i, el));

		++i;
	}

	std::vector<std::pair<bool, ImVec2>> result(points.size());
	// init all with false outers.
	std::ranges::transform(v, result.begin(), [](const ImVec2& p) { return std::make_pair(false, p); });
	// fill possible outer points.
	std::ranges::for_each(v, [&](const auto& el)
		{
			size_t i = &el - v.data();
			if (std::ranges::find_if(hull, [i](const auto& pair) { return i == pair.first; }) != hull.end())
				result.at(i) = { true, v[i] };
		});

	return result;
}

void removeClosePoints(std::vector<std::pair<bool, ImVec2>>& points, float threshold)
{
	// I hate the formatting of lambdas in vs, please don't reformat this
	std::erase_if(points, [&](const auto& p1) 
	{
		if (p1.first) // skip outer
			return false;

		return std::ranges::any_of(points, [&](const auto& p2)
		{
			if (p2.first)
				return false;

			const ImVec2 between = p2.second - p1.second;
			const float dist = std::sqrt(ImLengthSqr(between));

			return dist < threshold && &p1 != &p2;
		});
	});
}

void MolotovDraw::draw()
{
	if (!vars::visuals->world->molotov->enabled)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::GRENADE_PROJECTILES))
	{
		if (classID != CInferno)
			continue;

		auto molotov = reinterpret_cast<Inferno_t*>(entity);

		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/cstrike15/Effects/inferno.cpp
		// here you can see ratios and everything

		float time = molotov->spawnTime() + molotov->expireTime() - memory::interfaces::globalVars->m_curtime;
		float scale = time / molotov->expireTime();

		const auto& origin = molotov->absOrigin();
		constexpr float molotovRadius = 60; // 30 * 2

		Color col = vars::visuals->world->molotov->color();

		std::vector<Vec3> molotovWorldPoints;
		for (auto i = 0; i <= molotov->m_fireCount(); i++)
		{		
			auto pos = origin + molotov->getInfernoPos(i);
			molotovWorldPoints.push_back(pos);
		}

		// applying a very general fix for every part of possible angle.
		static const auto extraFlames = [] 
		{
			constexpr auto extraPoints = 4;
			constexpr float step = math::PI_2 / extraPoints;

			std::array<Vec3, extraPoints> points;
			for (float angle = 0.0f; auto& el : points)
			{
				el = Vec3{ molotovRadius * std::cos(angle), molotovRadius * std::sin(angle), 0.0f };
				angle += step;
			}

			return points;
		}();

		std::vector<ImVec2> points;
        for (const auto& el : molotovWorldPoints)
        {
			for (const auto& flame : extraFlames)
			{
				if (ImVec2 s; imRender.worldToScreen(el + flame, s))
					points.push_back(s);
			}
        }
		
		auto hull = grahamScanIndicies(points);
		if (!hull.has_value())
			continue;

		std::vector<ImVec2> hullOuter;
		for (const auto& [outer, coord] : hull.value())
		{
			if (outer)
				hullOuter.push_back(coord);
		}

		if (hullOuter.size() < 3) // can't even contrcut a triangle of edges
			continue;

		// bit hardocding to give acceptable treshold
		constexpr float acceptableScaled = 100.0f;
		constexpr float minDist = 20.0f;
		// very hardcoded scaling, works kind of well, or not really...
		const float test = game::localPlayer->absOrigin().distToMeters(origin) * 7;
		const float testingScale = std::max(std::min(acceptableScaled - test, acceptableScaled), minDist);
		removeClosePoints(hull.value(), testingScale);

		// todo: edit triangulation in some time to pass concept template of vec2 with floating point fields
		std::vector<float> triangPoints;
		for (const auto& el : hull.value())
		{
			triangPoints.emplace_back(el.second.x);
			triangPoints.emplace_back(el.second.y);
		}

		const delaunator::Delaunator<float> d{ triangPoints };

		if (vars::visuals->world->molotov->triangulation && d.triangles.size() > 6)
		{
			for (std::size_t i = 0; i < d.triangles.size(); i += 3)
			{
				ImVec2 p1{ d.coords.at(2 * d.triangles.at(i)), d.coords.at(2 * d.triangles.at(i) + 1)};
				ImVec2 p2{ d.coords.at(2 * d.triangles.at(i + 1)), d.coords.at(2 * d.triangles.at(i + 1) + 1) };
				ImVec2 p3{ d.coords.at(2 * d.triangles.at(i + 2)), d.coords.at(2 * d.triangles.at(i + 2) + 1) };

				imRender.drawTriangle(p1, p2, p3, Colors::White);
			}
		}

        imRender.drawPolyGon(hullOuter, vars::visuals->world->molotov->color());

		auto centerPolygon = [hullOuter, hullsize = hullOuter.size()]()
		{
			ImVec2 centre{};
			for (const auto& el : hullOuter)
			{
				centre.x += el.x;
				centre.y += el.y;
			}
	
			return centre / static_cast<float>(hullsize);
		};
		
		static float size = ImFonts::tahoma14->FontSize;
		const auto centerPoint = centerPolygon();
		imRender.drawProgressRing(centerPoint.x, centerPoint.y, 25, 32, -90.0f, scale, 5.0f, Colors::LightBlue);
		imRender.text(centerPoint.x, centerPoint.y - (size / 2.0f), ImFonts::tahoma14, std::format("{:.2f}s", time), true, Colors::White);
	}
}