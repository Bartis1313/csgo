#include "crosshair.hpp"

#include "../../../../../SDK/IVEngineClient.hpp"
#include "../../../../../SDK/IEngineTrace.hpp"
#include "../../../../../SDK/ICvar.hpp"
#include "../../../../../SDK/ConVar.hpp"
#include "../../../../../SDK/IWeapon.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/utilities.hpp"
#include "../../../../../utilities/math/math.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"

void Crosshair::init()
{
	m_scale = interfaces::cvar->findVar(XOR("cl_crosshair_recoil"));
}

void Crosshair::draw()
{
	int cfgCross = config.get<int>(vars.iCrosshair);

	m_scale->setValue(cfgCross == E2T(CrossHairTypes::ENGINE) ? true : false);

	if (!cfgCross)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isConnected())
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2.0f;
	y /= 2.0f;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	auto getPunchPos = [=]()
	{
		Vector angle;
		interfaces::engine->getViewAngles(angle);
		angle += game::localPlayer->m_aimPunchAngle() * m_scale->getFloat();

		auto start = game::localPlayer->getEyePos();
		auto end = start + math::angleVec(angle) * weapon->getWpnInfo()->m_range;

		return end;
	};

	switch (cfgCross)
	{
	case E2T(CrossHairTypes::STATIC):
	{
		float moveCross = 8.0f;

		imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::Black, 3.0f);
		imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::Black, 3.0f);

		moveCross -= 1.5f;

		imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::LightBlue, 1.0f);
		imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::LightBlue, 1.0f);
		break;
	}
	case E2T(CrossHairTypes::RECOIL):
	{
		if (Vector2D endScreen; imRender.worldToScreen(getPunchPos(), endScreen))
		{
			int x = endScreen.x;
			int y = endScreen.y;

			float moveCross = 8.0f;

			imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::Black, 3.0f);
			imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::Black, 3.0f);

			moveCross -= 1.5f;

			imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::LightBlue, 1.0f);
			imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::LightBlue, 1.0f);
		}
		break;
	}
	case E2T(CrossHairTypes::SPREAD):
	{
		if (Vector2D endScreen; imRender.worldToScreen(getPunchPos(), endScreen))
		{
			// this is game's logic how to do it
			/*float spread = weapon->getSpread();
			float inaccuracy = weapon->getInaccuracy();
			float scaledSpread = ((inaccuracy + spread) * 320.0f / std::tan(DEG2RAD(globals::FOV) / 2.0f));
			int radiusSpread = scaledSpread * y / 480.0f;

			if (game::localPlayer->m_vecVelocity().length2D() > 0.0f)
				radiusSpread = inaccuracy * 1000.0f;*/
			float inaccuracy = weapon->getInaccuracy();
			int radiusSpread = inaccuracy * 1000.0f;
			int x = endScreen.x;
			int y = endScreen.y;

			imRender.drawCircle(x, y, radiusSpread, 32, Colors::Black);
			imRender.drawCircleFilled(x, y, radiusSpread, 32, Colors::LightBlue.getColorEditAlpha(0.2f));
		}
		break;
	}
	default:
		break;
	}
}
