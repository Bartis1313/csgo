#include "bunnyhop.hpp"
#include "../../game.hpp"

enum movetypes
{
	NOCLIP = 8,
	LADDER = 9
};

void bunnyhop::run(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bBunnyHop))
		return;

	if (!game::localPlayer)
		return;

	if (game::localPlayer->m_nRenderMode() == NOCLIP || game::localPlayer->m_nRenderMode() == LADDER)
		return;

	if (!(cmd->m_buttons & IN_JUMP))
		return;

	if (!(game::localPlayer->m_fFlags() & FL_ONGROUND))
	{
		cmd->m_buttons &= ~IN_JUMP;
	}
}

void bunnyhop::strafe(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bAutoStrafe))
		return;

	if (!game::localPlayer)
		return;

	if (game::localPlayer->m_fFlags() & FL_ONGROUND)
		return;

	if (cmd->m_mousedx > 0 && cmd->m_buttons & IN_MOVERIGHT && cmd->m_buttons & IN_MOVELEFT)
		cmd->m_sidemove = -450.0f;
	else if (cmd->m_mousedx < 0 && cmd->m_buttons & IN_MOVELEFT && cmd->m_buttons & IN_MOVERIGHT)
		cmd->m_sidemove = 450.0f;
}