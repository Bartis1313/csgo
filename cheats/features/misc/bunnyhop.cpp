#include "bunnyhop.hpp"
#include "../../menu/vars.hpp"
#include "../../../utilities/utilities.hpp"
#include "../../../SDK/CUserCmd.hpp"

enum movetypes
{
	NOCLIP = 8,
	LADDER = 9
};

void bunnyhop::run(CUserCmd* cmd)
{
	if (!vars::bBunnyHop)
		return;

	if (!game::localPlayer)
		return;

	if (game::localPlayer->moveType() == NOCLIP || game::localPlayer->moveType() == LADDER)
		return;

	if (!(game::localPlayer->m_fFlags() & FL_ONGROUND))
	{
		cmd->m_forwardmove = 0.0f;

		if (cmd->m_mousedx < 0)
		{
			cmd->m_sidemove = -450.0f;
		}
		else if (cmd->m_mousedy > 0)
		{
			cmd->m_sidemove = 450.0f;
		}
	}
}

void bunnyhop::strafe(CUserCmd* cmd)
{
	if (!vars::bBunnyHop)
		return;

	if (!game::localPlayer)
		return;

	if (game::localPlayer->moveType() == NOCLIP || game::localPlayer->moveType() == LADDER)
		return;

	if (cmd->m_buttons & IN_JUMP)
	{
		if (!(game::localPlayer->m_fFlags() & FL_ONGROUND))
			cmd->m_buttons &= ~IN_JUMP;

		if (game::localPlayer->m_vecVelocity().Length() <= 50)
		{
			cmd->m_buttons = 450.f;
		}
	}
}