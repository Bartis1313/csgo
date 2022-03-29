#include "bunnyhop.hpp"

#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/ICvar.hpp"

#include "../../../config/vars.hpp"
#include "../../game.hpp"

enum movetypes
{
	NOCLIP = 8,
	LADDER = 9
};

void Bunnyhop::run(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bBunnyHop))
		return;

	if (!game::localPlayer)
		return;

	if (game::localPlayer->m_nRenderMode() == NOCLIP || game::localPlayer->m_nRenderMode() == LADDER)
		return;

	if (!(cmd->m_buttons & IN_JUMP))
		return;

	if (game::localPlayer->isInAir())
	{
		cmd->m_buttons &= ~IN_JUMP;
	}
}

void Bunnyhop::strafe(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bAutoStrafe))
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isInAir())
		return;

	const static auto cl_sidespeed = interfaces::cvar->findVar("cl_sidespeed")->getFloat();

	if (cmd->m_mousedx > 0 && cmd->m_buttons & IN_MOVERIGHT && cmd->m_buttons & IN_MOVELEFT)
		cmd->m_sidemove = -cl_sidespeed;
	else if (cmd->m_mousedx < 0 && cmd->m_buttons & IN_MOVELEFT && cmd->m_buttons & IN_MOVERIGHT)
		cmd->m_sidemove = cl_sidespeed;
}