#include "RCS.hpp"

#include "../aimbot/aimbot.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

#include <cheats/game/game.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

void RCS::init()
{
	m_scale = memory::interfaces::cvar->findVar("weapon_recoil_scale");
}

void RCS::run(CUserCmd* cmd)
{
	auto cfg = g_Aimbot->getCachedConfig();

	if (!cfg.rcs)
		return;

	if (!game::isAvailable())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();

	if (!weapon)
		return;

	if (weapon->isNonAimable())
		return;

	if (weapon->isSniper())
		return;

	prepare(cmd);
}

void RCS::prepare(CUserCmd* cmd)
{
	auto cfg = g_Aimbot->getCachedConfig();

	static auto oldPunch = game::getFixedPunch() * m_scale->getFloat();
	if (game::localPlayer()->m_iShotsFired() > 1 && cmd->m_buttons & IN_ATTACK)
	{
		auto punch = game::getFixedPunch() * m_scale->getFloat();

		punch[Coord::X] *= cfg.rcsX;
		punch[Coord::Y] *= cfg.rcsY;

		auto toMove = cmd->m_viewangles += (oldPunch - punch);
		toMove.clamp();

		memory::interfaces::engine->setViewAngles(toMove);

		oldPunch = punch;
	}
	else
	{
		auto punch = game::getFixedPunch() * m_scale->getFloat();
		punch[Coord::X] *= cfg.rcsX;
		punch[Coord::Y] *= cfg.rcsY;

		oldPunch = punch;
	}
}
