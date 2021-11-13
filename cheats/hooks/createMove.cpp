#include "hooks.hpp"
#include "../game.hpp"
#include <cstdint>
#include "../menu/vars.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/prediction/prediction.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/misc/bunnyhop.hpp"
#include "../features/aimbot/triggerbot.hpp"

bool __stdcall hooks::createMove::hooked(float inputFrame, CUserCmd* cmd)
{	
	original(inputFrame, cmd);

	if (!cmd || !cmd->m_commandNumber)
		return false;

	game::localPlayer = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getLocalPlayer()));

	// thanks for reminding me https://github.com/Bartis1313/csgo/issues/4
	if(original(inputFrame, cmd))
		interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	game::serverTime(cmd);

	bunnyhop::run(cmd);
	bunnyhop::strafe(cmd);

	prediction::start(cmd);
	{
		backtrack::run(cmd);
		legitbot::run(cmd);
		triggerbot::run(cmd);
	}	
	prediction::end();

	return false;
}