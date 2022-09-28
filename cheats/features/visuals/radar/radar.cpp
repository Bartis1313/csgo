#include "radar.hpp"

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

void Radar::init()
{
	
}

Vec2 Radar::entToRadar(const Vec3& eye, const Vec3& angles, const Vec3& entPos, const Vec2& pos, const Vec2& size, const float scale, bool clipRanges)
{
	float dotThickness = config.get<float>(vars.fRadarThickness);

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
		if (!config.get<bool>(vars.bRadarRanges))
		{
			if (dotX < pos[Coord::X])
				return {}; // this is zero vector

			if (dotX > pos[Coord::Y] + size[Coord::X]- dotThickness)
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
		console.log(TypeLogs::LOG_INFO, "Created map texture from path: {}", path);
	else
	{
		console.log(TypeLogs::LOG_ERR, "Creating map texture from path failed, code: {}", hr);
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
	float scale = config.get<float>(vars.fRadarScale);

	auto p1 = entToRadar(myEye, ang, poses.at(0), m_drawPos, m_drawSize, scale, false).toImVec();
	auto p2 = entToRadar(myEye, ang, poses.at(1), m_drawPos, m_drawSize, scale, false).toImVec();
	auto p3 = entToRadar(myEye, ang, poses.at(2), m_drawPos, m_drawSize, scale, false).toImVec();
	auto p4 = entToRadar(myEye, ang, poses.at(3), m_drawPos, m_drawSize, scale, false).toImVec();

	imRenderWindow.getDrawList()->AddImageQuad(m_mapTexture, p1, p2, p3, p4);
}

void Radar::draw()
{
	bool& ref = config.getRef<bool>(vars.bRadar);
	if (!ref)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	float size = config.get<float>(vars.fRadarSize);
	ImGui::SetNextWindowSize({ size, size });
	if (ImGui::Begin(XOR("Radar"), &ref, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		imRenderWindow.addList();

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

		for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++) // uisng cache might be not so much updated each frame, so I am using standard way
		{
			auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

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

			const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vec2{}, Vec2{ imRenderWindow.getWidth(), imRenderWindow.getHeight() }, config.get<float>(vars.fRadarScale), true);

			auto entYaw = ent->m_angEyeAngles()[Coord::Y];

			if (entYaw < 0.0f)
				entYaw = 360.0f + entYaw;

			const auto rotated = 270.0f - entYaw + ang[Coord::Y];

			auto dotRad = config.get<float>(vars.fRadarLenght);

			const auto finalX = dotRad * std::cos(math::DEG2RAD(rotated));
			const auto finalY = dotRad * std::sin(math::DEG2RAD(rotated));

			if (config.get<bool>(vars.bRadarRanges) ? true : !entRotatedPos.isZero())
			{
				auto dotThickness = config.get<float>(vars.fRadarThickness);

				CfgColor colLine = config.get<CfgColor>(vars.cRadarLine);
				imRenderWindow.drawLine(entRotatedPos[Coord::X] - 1, entRotatedPos[Coord::Y] - 1, entRotatedPos[Coord::X] + finalX, entRotatedPos[Coord::Y] + finalY,
					game::localPlayer->isPossibleToSee(ent, ent->getBonePos(8)) ? colLine.getColor() : colLine.getColor().getColorEditAlpha(0.5f));
				CfgColor colPlayer = config.get<CfgColor>(vars.cRadarPlayer);
				imRenderWindow.drawCircleFilled(entRotatedPos[Coord::X], entRotatedPos[Coord::Y], dotThickness, 32,
					game::localPlayer->isPossibleToSee(ent, ent->getBonePos(8)) ? colPlayer.getColor() : colPlayer.getColor().getColorEditAlpha(0.5f));

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

	g_Radar.m_inited = false;
}
