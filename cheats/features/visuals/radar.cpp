#include "radar.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include "../../globals.hpp"
#include <format>

constexpr auto dotThickness = 5;
constexpr auto dotRad = 20.0f;

#define VEC2D_NONE Vector2D(0.0f, 0.0f);

Vector2D radar::entToRadar(const Vector& eye, const Vector& angles, const Vector& entPos, const Vector2D& pos, const Vector2D& size, const float scale)
{
	float directionX, directionY;
	float dotX, dotY;

	directionX = -(entPos.y - eye.y);
	directionY = entPos.x - eye.x;

	auto yawDeg = angles.y - 90.0f;
	// calculate dots of radian and return correct view
	const auto yawToRadian = DEG2RAD(yawDeg);
	dotX = (directionY * std::cos(yawToRadian) - directionX * std::sin(yawToRadian)) / dotRad;
	dotY = (directionY * std::sin(yawToRadian) + directionX * std::cos(yawToRadian)) / dotRad;
	// return correct scale, it zooms in/out depends what value is thrown
	dotX *= scale;
	dotY *= scale;
	// correct it for our center screen of rectangle radar
	dotX += size.x / 2;
	dotY += size.y / 2;

	// do not draw out of range
	if (dotX < dotThickness)
		return VEC2D_NONE;

	if (dotX > size.x)
		return VEC2D_NONE;

	if (dotY < dotThickness)
		return VEC2D_NONE;

	if (dotY > size.y)
		return VEC2D_NONE;

	// again correct for out center...
	dotX += pos.x;
	dotY += pos.y;

	// should be just 2d vec, 3rd z value for 3d is empty because radar is a normal rectnagle -> 2D
	return Vector2D(dotX, dotY);
}

void radar::run()
{
	if (!config.get<bool>(vars.bRadar))
		return;

	if (!interfaces::engine->isInGame())
		return;

	// boi is angry so i give him localPlayer here
	auto localPlayer = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getLocalPlayer()));

	if (!localPlayer)
		return;

	int x = globals::screenX;
	int y = globals::screenY;
	static int centerx = x / 2 - 20;
	static int centery = y - 110;
	static int size = 90;

	render::drawFilledRect(centerx - size, centery - size, 2 * size, 2 * size, Color(128, 128, 128, 190));
	render::drawOutlineRect(centerx - size - 1, centery - size - 1, 2 * size + 1, 2 * size + 1, Colors::Black);

	render::text(centerx, centery - size - 14, fonts::tahoma, XOR("Radar"), true, Colors::LightBlue);

	render::drawFilledRect(centerx - 5, centery - 1, 11, 3, Colors::Black);
	render::drawFilledRect(centerx - 1, centery - 5, 3, 11, Colors::Black);

	render::drawLine(centerx - 4, centery, centerx + 5, centery, Colors::Green);
	render::drawLine(centerx, centery + 4, centerx, centery - 5, Colors::Green);
	
	// don't use this, there is needed trigonometry
	/*auto endXhelper = (globals::FOV > 90) ? -90 : -globals::FOV;
	auto endYhelper = (globals::FOV > 90) ? globals::FOV - 90 : 0;

	render::drawLine(Vector2D(centerx, centery), Vector2D(centerx - endXhelper, centery - size + endYhelper), Colors::Black);*/

	const auto myEye = localPlayer->getEyePos();
	Vector ang = {};
	interfaces::engine->getViewAngles(ang);

	for (int i = 1; i < interfaces::globalVars->m_maxClients; i++)
	{
		auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!ent)
			continue;

		if (ent->isDormant())
			continue;

		if (ent == localPlayer)
			continue;

		if (!ent->isAlive() || !game::localPlayer->isAlive())
			continue;

		if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vector2D(centerx - size, centery - size), Vector2D(2.0f * size, 2.0f * size), 1.8f);

		// must be better way, idk how to make it easier
		auto entYaw = ent->m_angEyeAngles().y;

		if (entYaw < 0.0f)
			entYaw = 360.0f + entYaw;

		const auto rotated = 270.0f - entYaw + ang.y;

		// this arg should not be dotRad in this case, as value gets higher line too
		const auto finalX = dotRad * std::cos(DEG2RAD(rotated));
		const auto finalY = dotRad * std::sin(DEG2RAD(rotated));

		if (!entRotatedPos.IsZero())
		{
			render::drawLine(entRotatedPos.x - 1, entRotatedPos.y - 1, entRotatedPos.x + finalX, entRotatedPos.y + finalY, Colors::White);
			render::drawCircleFilled(entRotatedPos.x, entRotatedPos.y, dotThickness, 32, Color(255, 30, 110, 255));
			// magic - 4 due to text, and - 3 to correction
			render::text(entRotatedPos.x - 4, entRotatedPos.y - dotThickness - 3, fonts::smalle, std::format("{:.2f}m", utilities::distToMeters(localPlayer->absOrigin().DistTo(ent->absOrigin()))), false, Color(100, 20, 70, 255));
		}
	}
}