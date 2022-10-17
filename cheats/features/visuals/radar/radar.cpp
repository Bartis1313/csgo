#include "radar.hpp"

#include <features/cache/cache.hpp>

#include <SDK/IVEngineClient.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/MapStruct.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/math/math.hpp>
#include <utilities/res.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <dependencies/ImGui/imgui.h>
#include <dependencies/ImGui/imgui_internal.h>

#include <d3dx9.h>

Vec2 Radar::entToRadar(const Vec3& eye, const Vec3& angles, const Vec3& entPos, const Vec2& pos, const Vec2& size, const float scale, bool clipRanges)
{
	float dotThickness = vars::misc->radar->thickness;

	float directionX = entPos[Coord::X] - eye[Coord::X];
	float directionY = -(entPos[Coord::Y] - eye[Coord::Y]);

	auto yawDeg = angles[Coord::Y] - 90.0f;
	// calculate dots of radian and return correct view
	const auto yawToRadian = math::DEG2RAD(yawDeg);
	float dotX = (directionX * std::cos(yawToRadian) - directionY * std::sin(yawToRadian)) / 20.0f;
	float dotY = (directionX * std::sin(yawToRadian) + directionY * std::cos(yawToRadian)) / 20.0f;
	// return correct scale, it zooms in/out depends what value is thrown
	dotX *= scale;
	dotY *= scale;
	// correct it for our center screen of rectangle radar
	dotX += size[Coord::X] / 2.0f;
	dotY += size[Coord::Y] / 2.0f;

	// do not draw out of range, added pos, even we pass 0, but for clarity
	if (clipRanges)
	{
		if (!vars::misc->radar->ranges)
		{
			if (dotX < pos[Coord::X])
				return {}; // this is zero vector

			if (dotX > pos[Coord::Y] + size[Coord::X] - dotThickness)
				return {};

			if (dotY < pos[Coord::Y])
				return {};

			if (dotY > pos[Coord::Y] + size[Coord::Y] - dotThickness)
				return {};
		}
		else
		{
			if (dotX < pos[Coord::X])
				dotX = pos[Coord::X];

			if (dotX > pos[Coord::Y] + size[Coord::X] - dotThickness)
				dotX = pos[Coord::Y] + size[Coord::X] - dotThickness;

			if (dotY < pos[Coord::Y])
				dotY = pos[Coord::Y];

			if (dotY > pos[Coord::Y] + size[Coord::Y] - dotThickness)
				dotY = pos[Coord::Y] + size[Coord::Y] - dotThickness;
		}
	}

	// again correct for out center...
	dotX += pos[Coord::X];
	dotY += pos[Coord::Y];

	return Vec2{ dotX, dotY };
}

void Radar::manuallyInitPos()
{
	if (!game::isAvailable())
		return;

	const auto map = reinterpret_cast<MapStruct*>(game::findHudElement(XOR("CCSGO_MapOverview")));
	m_pos = map->m_origin;
	m_scale = map->m_scale;

	m_inited = true;
}

bool Radar::manuallyInitTexture()
{
	if (!game::isAvailable())
		return false;

	std::string levelName = interfaces::engine->getLevelName();

	// not really working for workshops
	if (auto place = levelName.rfind('/'); place != std::string::npos)
		levelName = levelName.substr(place + 1, levelName.size());

	std::string path = FORMAT(XOR("csgo\\resource\\overviews\\{}_radar.dds"), levelName);

	// prob not supported format
	/*Resource res{ path };
	if (res.getTexture())
		m_mapTexture = res.getTexture();
	else
		return false;*/

	if (auto hr = D3DXCreateTextureFromFileA(interfaces::dx9Device, path.c_str(), &m_mapTexture); hr == D3D_OK)
		LOG_INFO(XOR("Created map texture from path: {}"), path);
	else
	{
		LOG_ERR(XOR("Creating map texture from path failed, code: {}"), hr);
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

	auto map = getMapPos();

	// square
	float size = map.m_scale;
	std::array poses =
	{
		Vec3{ m_pos[Coord::X], m_pos[Coord::Y], 0.0f },
		Vec3{ m_pos[Coord::X] + size, m_pos[Coord::Y], 0.0f },
		Vec3{ m_pos[Coord::X] + size, m_pos[Coord::Y] - size, 0.0f },
		Vec3{ m_pos[Coord::X], m_pos[Coord::Y] - size, 0.0f },
	};

	const auto myEye = game::localPlayer->getEyePos();
	Vec3 ang = {};
	interfaces::engine->getViewAngles(ang);
	float scale = vars::misc->radar->scale;

	auto p1 = entToRadar(myEye, ang, poses.at(0), m_drawPos, m_drawSize, scale, false).toImVec();
	auto p2 = entToRadar(myEye, ang, poses.at(1), m_drawPos, m_drawSize, scale, false).toImVec();
	auto p3 = entToRadar(myEye, ang, poses.at(2), m_drawPos, m_drawSize, scale, false).toImVec();
	auto p4 = entToRadar(myEye, ang, poses.at(3), m_drawPos, m_drawSize, scale, false).toImVec();

	imRenderWindow.getDrawList()->AddImageQuad(m_mapTexture, p1, p2, p3, p4);
}

void Radar::draw()
{
	if (!vars::misc->radar->enabled)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	float size = vars::misc->radar->size;
	ImGui::SetNextWindowSize({ size, size });
	if (ImGui::Begin(XOR("Radar"), &vars::misc->radar->enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		imRenderWindow.addList();

		m_drawSize = Vec2{ imRenderWindow.getRect().x, imRenderWindow.getRect().y };
		m_drawPos = Vec2{ imRenderWindow.getPos().x, imRenderWindow.getPos().y };

		const auto myEye = game::localPlayer->getEyePos();
		Vec3 ang = {};
		interfaces::engine->getViewAngles(ang);

		auto rect = imRenderWindow.getRect();
		float scaledFov = globals::FOV / 5.0f;
		// assume calculation is only in straight line representing 2D, so max = 180 deg, and this is not correct in 100%, idk better solution
		float fovAddon = rect.y / std::tan(math::DEG2RAD((180.0f - (globals::FOV + scaledFov)) / 2.0f));

		float middleX = rect.x / 2.0f;
		float middleY = rect.y / 2.0f;

		drawMap();

		// triangles representing fov view
		imRenderWindow.drawTriangleFilled({ middleX, middleY }, { middleX, 0.0f }, { middleX + fovAddon / 2.0f, 0.0f }, Colors::White.getColorEditAlpha(0.4f));
		imRenderWindow.drawTriangleFilled({ middleX, middleY }, { middleX, 0.0f }, { middleX - fovAddon / 2.0f, 0.0f }, Colors::White.getColorEditAlpha(0.4f));

		// fov lines
		imRenderWindow.drawLine({ middleX + fovAddon / 2.0f, 0.0f }, { middleX, middleY }, Colors::White);
		imRenderWindow.drawLine({ middleX - fovAddon / 2.0f, 0.0f }, { middleX, middleY }, Colors::White);

		// normal cross lines
		imRenderWindow.drawLine({ 0.0f, middleY }, { rect.x, middleY }, Colors::White);
		imRenderWindow.drawLine({ middleX, 0.0f }, { middleX, rect.y }, Colors::White);

		// draw small circle where are we
		imRenderWindow.drawCircleFilled(middleX, middleY, 5.0f, 12, Colors::Green);

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

			const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vec2{}, Vec2{ imRenderWindow.getWidth(), imRenderWindow.getHeight() },
				vars::misc->radar->scale, true);

			// or use calcangle, this will be faster though
			auto entYaw = ent->m_angEyeAngles()[Coord::Y];

			if (entYaw < 0.0f)
				entYaw = 360.0f + entYaw;

			const auto rotated = 270.0f - entYaw + ang[Coord::Y];

			auto dotRad = vars::misc->radar->length;

			const auto finalX = dotRad * std::cos(math::DEG2RAD(rotated));
			const auto finalY = dotRad * std::sin(math::DEG2RAD(rotated));

			if (vars::misc->radar->ranges ? true : !entRotatedPos.isZero())
			{
				auto dotThickness = vars::misc->radar->thickness;

				imRenderWindow.drawLine(entRotatedPos[Coord::X] - 1, entRotatedPos[Coord::Y] - 1, entRotatedPos[Coord::X] + finalX,
					entRotatedPos[Coord::Y] + finalY, vars::misc->radar->colorLine());
				
				imRenderWindow.drawCircleFilled(entRotatedPos[Coord::X], entRotatedPos[Coord::Y], dotThickness, 32,
					vars::misc->radar->colorPlayer());

				//imRenderWindow.drawTriangleFilled(entRotatedPos[Coord::X], entRotatedPos[Coord::Y], dotThickness,
				//	dotThickness, rotated, vars::misc->radar->colorPlayer());

			}
		}
		imRenderWindow.end();
		ImGui::End();
	}
}

void RadarSizeHelper::init()
{

}

void RadarSizeHelper::run(MapStruct* map)
{
	/*g_Radar.m_pos = map->m_origin;
	g_Radar.m_scale = map->m_scale;*/

	// something weird, the mapstruct for hook now has a 276 pad but it still responds with wrong values here
	// although by CCSGO_MapOverview it has 256 still...
	// so we trick it to load manually on new map

	g_Radar->m_inited = false;
}
