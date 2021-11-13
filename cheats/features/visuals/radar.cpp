#include "radar.hpp"
#include "../../game.hpp"
#include "../../menu/vars.hpp"
#include <format>

static constexpr auto dotThickness = 5;
static constexpr auto dotRad = 20.0f;

Vector2D radar::entToRadar(Vector eye, Vector angles, Vector entPos, Vector2D pos, Vector2D size, float scale, bool& check)
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
		check = false;

	if (dotX > size.x)
		check = false;

	if (dotY < dotThickness)
		check = false;

	if (dotY > size.y)
		check = false;

	// again correct for out center...
	dotX += pos.x;
	dotY += pos.y;

	// because it may fail on start of new game
	if (!game::localPlayer)
		check = false;

	// should be just 2d vec, 3rd z value for 3d is empty because radar is a normal rectnagle -> 2D
	return Vector2D(dotX, dotY);
}

void radar::run()
{
	if (!vars::bRadar)
		return;

	if (!interfaces::engine->isInGame())
		return;

	if (!game::localPlayer)
		return;

	int x, y;
	interfaces::engine->getScreenSize(x, y);
	int centerx = x / 2 - 20;
	int centery = y - 110;
	int size = 90;

	render::drawTrapezFilled(Vector2D(centerx - size, centery - size), Vector2D(centerx + size, centery - size),
		Vector2D(centerx + size, centery + size), Vector2D(centerx - size, centery + size), Colors::Grey);

	render::drawTrapezOutline(Vector2D(centerx - size - 1, centery - size - 1), Vector2D(centerx + size + 1, centery - size - 1),
		Vector2D(centerx + size + 1, centery + size + 1), Vector2D(centerx - size - 1, centery + size + 1), Colors::Black);

	int toAlign = render::getTextSize(fonts::tahoma, "Radar");
	render::text(centerx - toAlign / 2, centery - size - 10, fonts::tahoma, "Radar", false, Colors::LightBlue);

	render::drawFilledRect(centerx, centery - size, 1, 2 * size, Color(0, 0, 0, 120));
	render::drawFilledRect(centerx - size, centery, 2 * size, 1, Color(0, 0, 0, 120));

	const auto myEye = game::localPlayer->getEyePos();
	Vector ang = {};
	interfaces::engine->getViewAngles(ang);

	for (int i = 1; i < interfaces::globalVars->m_maxClients; i++)
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

		bool check = true;
		const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vector2D(centerx - size, centery - size), Vector2D(2.0f * size, 2.0f * size), 1.8f, check);

		// must be better way
		auto entYaw = ent->m_angEyeAngles().y;

		if (entYaw < 0)
			entYaw = 180 + ((180 + entYaw));

		const auto rotated = (270 - entYaw) + ang.y;

		// this arg should not be dotRad in this case, as value gets higher line too
		const auto finalX = dotRad * std::cos(DEG2RAD(rotated));
		const auto finalY = dotRad * std::sin(DEG2RAD(rotated));

		if (check)
		{
			render::drawLine(entRotatedPos.x - 1, entRotatedPos.y - 1, entRotatedPos.x + finalX, entRotatedPos.y + finalY, Colors::White);
			render::drawCircleFilled(entRotatedPos.x, entRotatedPos.y, dotThickness, 32, Color(255, 30, 110, 255));
			// magic - 4 due to text, and - 3 to correction
			render::text(entRotatedPos.x - 4, entRotatedPos.y - dotThickness - 3, fonts::smalle, std::format("{:.2f}m", utilities::distToMeters(game::localPlayer->absOrigin().DistTo(ent->absOrigin()))), false, Color(100, 20, 70, 255));
		}
	}
}