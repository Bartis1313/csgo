#include "radar.hpp"

#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/math/Vector2D.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include "../../globals.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"

#include "../../../dependencies/ImGui/imgui.h"
#include "../../../dependencies/ImGui/imgui_internal.h"

Vector2D Radar::entToRadar(const Vector& eye, const Vector& angles, const Vector& entPos, const Vector2D& pos, const Vector2D& size, const float scale)
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
	dotX += size.x / 2.0f;
	dotY += size.y / 2.0f;

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

void Radar::run()
{
	bool& ref = config.getRef<bool>(vars.bRadar);
	if (!ref)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	if (ImGui::Begin(XOR("Radar"), &ref, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
	{
		imRenderWindow.addList();

		const auto myEye = game::localPlayer->getEyePos();
		Vector ang = {};
		interfaces::engine->getViewAngles(ang);

		auto rect = imRenderWindow.getRect();
		float scaledFov = globals::FOV / 5.0f;
		// assume calculation is only in straight line representing 2D, so max = 180 deg, and this is not correct in 100%, idk better solution
		float fovAddon = rect.y / std::tan(DEG2RAD((180.f - (globals::FOV + scaledFov)) / 2.0f));

		float middleX = rect.x / 2.0f;
		float middleY = rect.y / 2.0f;

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

			const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vector2D{}, Vector2D{ imRenderWindow.getWidth(), imRenderWindow.getHeight() }, config.get<float>(vars.fRadarScale));

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

				CfgColor colLine = config.get<CfgColor>(vars.cRadarLine);
				imRenderWindow.drawLine(entRotatedPos.x - 1, entRotatedPos.y - 1, entRotatedPos.x + finalX, entRotatedPos.y + finalY,
					game::localPlayer->isPossibleToSee(ent->getBonePos(8)) ? colLine.getColor() : colLine.getColor().getColorEditAlpha(0.5f));
				CfgColor colPlayer = config.get<CfgColor>(vars.cRadarPlayer);
				imRenderWindow.drawCircleFilled(entRotatedPos.x, entRotatedPos.y, dotThickness, 32,
					game::localPlayer->isPossibleToSee(ent->getBonePos(8)) ? colPlayer.getColor() : colPlayer.getColor().getColorEditAlpha(0.5f));

			}
		}
		ImGui::End();
	}
}