#include "radar.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include "../../globals.hpp"
#include "../../../dependencies/ImGui/imgui.h"
#include "../../../dependencies/ImGui/imgui_internal.h"
#include <format>

// TODO: those renders should be moved in some more obvious way to never put it in the pt

Vector2D radar::entToRadar(const Vector& eye, const Vector& angles, const Vector& entPos, const Vector2D& pos, const Vector2D& size, const float scale)
{
	auto dotThickness = config.get<float>(vars.fRadarThickness);

	float directionX, directionY;
	float dotX, dotY;

	directionX = -(entPos.y - eye.y);
	directionY = entPos.x - eye.x;

	auto yawDeg = angles.y - 90.0f;
	// calculate dots of radian and return correct view
	const auto yawToRadian = DEG2RAD(yawDeg);
	dotX = (directionY * std::cos(yawToRadian) - directionX * std::sin(yawToRadian)) / 20.0f;
	dotY = (directionY * std::sin(yawToRadian) + directionX * std::cos(yawToRadian)) / 20.0f;
	// return correct scale, it zooms in/out depends what value is thrown
	dotX *= scale;
	dotY *= scale;
	// correct it for our center screen of rectangle radar
	dotX += size.x / 2;
	dotY += size.y / 2;

	// do not draw out of range, added pos, even we pass 0, but for clarity
	if (!config.get<bool>(vars.bRadarRanges))
	{
		if (dotX < pos.x)
			return {}; // this is zero vector

		if (dotX > pos.y + size.x - dotThickness)
			return {};

		if (dotY < pos.y)
			return {};

		if (dotY > pos.y + size.y - dotThickness)
			return {};
	}
	else
	{
		if (dotX < pos.x)
			dotX = pos.x;

		if (dotX > pos.y + size.x - dotThickness)
			dotX = pos.y + size.x - dotThickness;

		if (dotY < pos.y)
			dotY = pos.y;

		if (dotY > pos.y + size.y - dotThickness)
			dotY = pos.y + size.y - dotThickness;
	}

	// again correct for out center...
	dotX += pos.x;
	dotY += pos.y;

	return { dotX, dotY };
}

void radar::run()
{
	if (bool& ref = config.getRef<bool>(vars.bRadar))
	{
		if (!game::localPlayer)
			return;

		if (!interfaces::engine->isInGame())
			return;

		if (ImGui::Begin("Radar", &ref))
		{
			imRenderWindow.addList();
			imRenderWindow.drawRectFilled(0, 0, imRenderWindow.m_rect.x, imRenderWindow.m_rect.y, Color(128, 128, 128, 190));
			imRenderWindow.drawRect(0, 0, imRenderWindow.m_rect.x + 1, imRenderWindow.m_rect.y + 1, Colors::Black);

			const auto myEye = game::localPlayer->getEyePos();
			Vector ang = {};
			interfaces::engine->getViewAngles(ang);

			for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
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

				if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
					continue;

				const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vector2D{}, Vector2D(imRenderWindow.m_rect.x, imRenderWindow.m_rect.y), config.get<float>(vars.fRadarScale));

				auto entYaw = ent->m_angEyeAngles().y;

				if (entYaw < 0.0f)
					entYaw = 360.0f + entYaw;

				const auto rotated = 270.0f - entYaw + ang.y;

				auto dotRad = config.get<float>(vars.fRadarLenght);

				const auto finalX = dotRad * std::cos(DEG2RAD(rotated));
				const auto finalY = dotRad * std::sin(DEG2RAD(rotated));

				if (config.get<bool>(vars.bRadarRanges) ? true : !entRotatedPos.isZero())
				{
					auto dotThickness = config.get<float>(vars.fRadarThickness);

					auto colLine = config.get<Color>(vars.cRadarLine);
					imRenderWindow.drawLine(entRotatedPos.x - 1, entRotatedPos.y - 1, entRotatedPos.x + finalX, entRotatedPos.y + finalY,
						game::localPlayer->isPossibleToSee(ent->getBonePosition(8)) ? colLine : colLine.getColorEditAlpha(0.5f));
					auto colPlayer = config.get<Color>(vars.cRadarPlayer);
					imRenderWindow.drawCircleFilled(entRotatedPos.x, entRotatedPos.y, dotThickness, 32,
						game::localPlayer->isPossibleToSee(ent->getBonePosition(8)) ? colPlayer : colPlayer.getColorEditAlpha(0.5f));
			
				}
			}
			ImGui::End();
		}
	}
}