#include "crosshair.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/math/math.hpp>
#include <render/render.hpp>

#include <cheats/hooks/paintTraverse.hpp>

namespace
{
	struct CrosshairHandler : hooks::PaintTraverse
	{
		CrosshairHandler()
		{
			this->registerInit(crosshair::init);
			this->registerRender(crosshair::draw);
			this->registerShutdown(crosshair::shutdown);
		}
	} crosshairHandler;
}

namespace crosshair
{
	IConVar* m_crosshair;
	IConVar* weapon_recoil_scale;
	IConVar* cl_crosshair_recoil;
}

void crosshair::init()
{
	m_crosshair = memory::interfaces::cvar->findVar("crosshair");
	cl_crosshair_recoil = memory::interfaces::cvar->findVar("cl_crosshair_recoil");
	weapon_recoil_scale = memory::interfaces::cvar->findVar("weapon_recoil_scale");
}

void crosshair::draw()
{
	int cfgCross = vars::misc->crosshair->index;
	if(!m_crosshair->getInt())
		m_crosshair->setValue(true);

	if (cl_crosshair_recoil->getInt())
		cl_crosshair_recoil->setValue(false);

	if (!cfgCross)
		return;

	cl_crosshair_recoil->setValue(cfgCross == E2T(CrossHairTypes::ENGINE) ? true : false);
	if (cl_crosshair_recoil->getInt())
		m_crosshair->setValue(true);
	else
		m_crosshair->setValue(false);

	if (!game::localPlayer)
		return;

	if (!memory::interfaces::engine->isConnected())
		return;

	if (!game::localPlayer->isAlive())
		return;

	float x = globals::screenX / 2.0f;
	float y = globals::screenY / 2.0f;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	auto getPunchPos = [=]()
	{
		Vec3 angle;
		memory::interfaces::engine->getViewAngles(angle);
		angle += game::localPlayer->m_aimPunchAngle() * weapon_recoil_scale->getFloat();

		auto start = game::localPlayer->getEyePos();
		auto end = start + math::angleVec(angle) * weapon->getWpnInfo()->m_range;

		return end;
	};

	switch (cfgCross)
	{
	case E2T(CrossHairTypes::STATIC):
	{
		float moveCross = 8.0f;

		ImRender::drawLine(x - moveCross, y, x + moveCross, y, Colors::Black, 3.0f);
		ImRender::drawLine(x, y - moveCross, x, y + moveCross, Colors::Black, 3.0f);

		moveCross -= 1.5f;

		ImRender::drawLine(x - moveCross, y, x + moveCross, y, Colors::LightBlue, 1.0f);
		ImRender::drawLine(x, y - moveCross, x, y + moveCross, Colors::LightBlue, 1.0f);
		break;
	}
	case E2T(CrossHairTypes::RECOIL):
	{
		if (ImVec2 endScreen; ImRender::worldToScreen(getPunchPos(), endScreen))
		{
			float x = endScreen.x;
			float y = endScreen.y;

			float moveCross = 8.0f;

			ImRender::drawLine(x - moveCross, y, x + moveCross, y, Colors::Black, 3.0f);
			ImRender::drawLine(x, y - moveCross, x, y + moveCross, Colors::Black, 3.0f);

			moveCross -= 1.5f;

			ImRender::drawLine(x - moveCross, y, x + moveCross, y, Colors::LightBlue, 1.0f);
			ImRender::drawLine(x, y - moveCross, x, y + moveCross, Colors::LightBlue, 1.0f);
		}
		break;
	}
	case E2T(CrossHairTypes::SPREAD):
	{
		if (ImVec2 endScreen; ImRender::worldToScreen(getPunchPos(), endScreen))
		{
			// this is game's logic how to do it
			/*float spread = weapon->getSpread();
			float inaccuracy = weapon->getInaccuracy();
			float scaledSpread = ((inaccuracy + spread) * 320.0f / std::tan(DEG2RAD(globals::FOV) / 2.0f));
			int radiusSpread = scaledSpread * y / 480.0f;

			if (game::localPlayer->m_vecVelocity().length2D() > 0.0f)
				radiusSpread = inaccuracy * 1000.0f;*/
			float inaccuracy = weapon->getInaccuracy();
			float radiusSpread = inaccuracy * 1000.0f;
			float x = endScreen.x;
			float y = endScreen.y;

			ImRender::drawCircle(x, y, radiusSpread, 32, Colors::Black);
			ImRender::drawCircleFilled(x, y, radiusSpread, 32, Colors::LightBlue.getColorEditAlpha(0.2f));
		}
		break;
	}
	default:
		break;
	}
}

void crosshair::shutdown()
{
	m_crosshair->setValue(true);
	cl_crosshair_recoil->setValue(false);
}
