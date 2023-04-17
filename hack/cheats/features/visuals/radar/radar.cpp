#include "radar.hpp"

#include <cheats/features/cache/cache.hpp>

#include <SDK/IVEngineClient.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/MapStruct.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <utilities/res.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <d3dx9.h>

ImVec2 Radar::entToRadar(const Vec3& eye, const Vec3& angles, const Vec3& entPos, const float scale, bool clipRanges)
{
	float dotThickness = vars::misc->radar->thickness;

	float directionX = entPos[Coord::X] - eye[Coord::X];
	float directionY = -(entPos[Coord::Y] - eye[Coord::Y]);

	const auto yawDeg = angles[Coord::Y] - 90.0f;
	// calculate dots of radian and return correct view
	const auto yawToRadian = math::DEG2RAD(yawDeg);
	const auto cosine = std::cos(yawToRadian);
	const auto sine = std::sin(yawToRadian);
	// this 20 is a magic scaling value, it seems to be the best and without need as configurable
	// game also does hardcoded scaling
	// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/cstrike15/Scaleform/HUD/sfhudradar.cpp#L184
	float dotX = (directionX * cosine - directionY * sine) / 20.0f;
	float dotY = (directionX * sine + directionY * cosine) / 20.0f;
	dotX *= scale;
	dotY *= scale;

	const auto size = ImGui::GetWindowSize();

	// correct it for our center screen of rectangle radar
	dotX += size.x / 2.0f;
	dotY += size.y / 2.0f;

	// do not draw out of range, added pos, even we pass 0, but for clarity
	if (clipRanges && vars::misc->radar->ranges)
	{
		dotX = std::clamp(dotX, dotThickness, size.x - dotThickness);
		dotY = std::clamp(dotY, dotThickness, size.y - dotThickness);
	}

	const auto pos = ImGui::GetWindowPos();
	dotX += pos.x;
	dotY += pos.y;

	return ImVec2{ dotX, dotY };
}

void Radar::manuallyInitPos()
{
	if (!game::isAvailable())
		return;

	const auto map = memory::Address<MapStruct*>{ game::findHudElement("CCSGO_MapOverview") }.sub(0x14);
	m_pos = map->m_origin;
	m_scale = map->m_scale;

	m_inited = true;
}

bool Radar::manuallyInitTexture()
{
	if (!game::isAvailable())
		return false;

	std::string levelName = memory::interfaces::engine->getLevelName();

	// not really working for workshops
	if (const auto place = levelName.rfind('/'); place != std::string::npos)
		levelName = levelName.substr(place + 1, levelName.size());

	const std::string path = std::format("csgo\\resource\\overviews\\{}_radar.dds", levelName);

	if (auto hr = D3DXCreateTextureFromFileA(memory::interfaces::dx9Device(), path.c_str(), &m_mapTexture); hr == D3D_OK)
		console::info("Created map texture from path: {}", path);
	else
	{
		console::error("Creating map texture from path failed, code: {}", hr);
		return false;
	}

	return true;
}

Radar::MapPos Radar::getMapPos() const
{
	return MapPos{ m_pos, m_scale * 1000.0f };
}

void Radar::drawMap()
{
	if (!m_inited)
		manuallyInitPos();

	const auto map = getMapPos();

	// square
	const float size = map.m_scale;
	const std::array poses =
	{
		Vec3{ m_pos[Coord::X], m_pos[Coord::Y], 0.0f },
		Vec3{ m_pos[Coord::X] + size, m_pos[Coord::Y], 0.0f },
		Vec3{ m_pos[Coord::X] + size, m_pos[Coord::Y] - size, 0.0f },
		Vec3{ m_pos[Coord::X], m_pos[Coord::Y] - size, 0.0f },
	};

	const auto myEye = game::localPlayer->getEyePos();
	const auto ang = game::getViewAngles();
	const float scale = vars::misc->radar->scale;

	auto p1 = entToRadar(myEye, ang, poses.at(0), scale, false);
	auto p2 = entToRadar(myEye, ang, poses.at(1), scale, false);
	auto p3 = entToRadar(myEye, ang, poses.at(2), scale, false);
	auto p4 = entToRadar(myEye, ang, poses.at(3), scale, false);

	ImGui::GetWindowDrawList()->AddImageQuad(m_mapTexture, p1, p2, p3, p4);
}

void Radar::draw()
{
	if (!vars::misc->radar->enabled)
		return;

	if (!game::localPlayer)
		return;

	if (!memory::interfaces::engine->isInGame())
		return;

	// force square sized window
	const auto maxSize = std::max(globals::screenX, globals::screenY);
	ImGui::SetNextWindowSizeConstraints(ImVec2{ 100.0f, 100.0f }, ImVec2{ static_cast<float>(maxSize), static_cast<float>(maxSize) },
		[](ImGuiSizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y; });
	if (ImGui::Begin("Radar", &vars::misc->radar->enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
	{
		const auto windowList = ImGui::GetWindowDrawList();
		const auto windowPos = ImGui::GetWindowPos();
		const auto windowSize = ImGui::GetWindowSize();

		const float middleX = windowSize.x / 2.0f;
		const float middleY = windowSize.y / 2.0f;

		drawMap();

		// draw small circle where are we
		drawing::CircleFilled{ 
			ImVec2{ windowPos.x + middleX, windowPos.y + middleY },
			5.0f, 12, Color::U32(Colors::Green) }.draw(windowList);

		const auto myEye = game::localPlayer->getEyePos();
		Vec3 viewAngle = game::getViewAngles();

		for(auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
		{
			auto ent = reinterpret_cast<Player_t*>(entity);

			if (!ent)
				continue;

			if (ent->isDormant())
				continue;

			if (ent == game::localPlayer)
				continue;

			if (!ent->isAlive() || !game::localPlayer->isAlive())
				continue;

			if (!ent->isOtherTeam(game::localPlayer()))
				continue;

			const auto entRotatedPos = entToRadar(myEye, viewAngle, ent->absOrigin(),
				vars::misc->radar->scale, true);

			const auto entYaw = math::normalizeYaw(ent->m_angEyeAngles()[Coord::Y]);

			// or use atan2 if 270 is magic for you
			const auto rotated = 270.0f - entYaw + viewAngle[Coord::Y];
			const auto dotRad = vars::misc->radar->length;

			const auto finalX = dotRad * std::cos(math::DEG2RAD(rotated));
			const auto finalY = dotRad * std::sin(math::DEG2RAD(rotated));

			const bool isZeroEnt = vars::misc->radar->ranges ? true : entRotatedPos.x != 0.0f && entRotatedPos.y != 0.0f;
			if (!isZeroEnt)
				return;

			if (vars::misc->radar->mode == E2T(RadarMode::TRIANGLE))
			{
				const auto dotThickness = vars::misc->radar->thickness;
				const float radian = math::DEG2RAD(rotated);
				const float radian90 = math::DEG2RAD(rotated + 90);

				const ImVec2 p1 = ImVec2{ (dotThickness / 2.0f) * std::cos(radian90) + entRotatedPos.x, (dotThickness / 2.0f) * std::sin(radian90) + entRotatedPos.y };
				const ImVec2 p2 = ImVec2{ (-dotThickness / 2.0f) * std::cos(radian90) + entRotatedPos.x, (-dotThickness / 2.0f) * std::sin(radian90) + entRotatedPos.y };
				const ImVec2 p3 = ImVec2{ dotThickness * std::cos(radian) + entRotatedPos.x, dotThickness * std::sin(radian) + entRotatedPos.y };
				drawing::TriangleFilled{ p1, p2, p3, Color::U32(vars::misc->radar->colorPlayer()) }.draw(windowList);
			}
			else if (vars::misc->radar->mode == E2T(RadarMode::CIRCLE))
			{
				const auto dotThickness = vars::misc->radar->thickness;
				const float radian = math::DEG2RAD(rotated);

				// we use p3 as a reference to triangles because we need optimal start where to draw based on enemy angles
				const ImVec2 p3 = ImVec2{ dotThickness * std::cos(radian) + entRotatedPos.x, dotThickness * std::sin(radian) + entRotatedPos.y };

				drawing::CircleFilled{ entRotatedPos, dotThickness, 32, Color::U32(vars::misc->radar->colorPlayer()) }.draw(windowList);
				drawing::Line{ p3, { p3.x + finalX, p3.y + finalY }, Color::U32(vars::misc->radar->colorLine()), 1.0f }.draw(windowList);
			}
		}
		
		ImGui::End();
	}
}

// important to call this after hook return
void RadarSizeHelper::run(MapStruct* map)
{
	g_Radar->m_pos = map->m_origin;
	g_Radar->m_scale = map->m_scale;

	g_Radar->m_inited = true;
}
