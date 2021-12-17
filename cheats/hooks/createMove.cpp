#include "hooks.hpp"
#include "../game.hpp"
#include "../menu/vars.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/prediction/prediction.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/misc/bunnyhop.hpp"
#include "../features/aimbot/triggerbot.hpp"
#include "../globals.hpp"
#include "../features/visuals/world.hpp"
#include "../features/misc/misc.hpp"

bool __stdcall hooks::createMove::hooked(float inputFrame, CUserCmd* cmd)
{	
	game::localPlayer = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getLocalPlayer()));

	if (!cmd || !cmd->m_commandNumber)
		original(inputFrame, cmd);

	// thanks for reminding me https://github.com/Bartis1313/csgo/issues/4
	if(original(inputFrame, cmd))
		interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	game::serverTime(cmd);
	bunnyhop::run(cmd);
	bunnyhop::strafe(cmd);

	uintptr_t* framePtr;
	__asm { \
		__asm mov framePtr, ebp \
	}
	auto& sendPacket = *reinterpret_cast<bool*>(*framePtr - 0x1C);

	prediction::start(cmd);
	{
		backtrack::run(cmd);
		legitbot::run(cmd);
		triggerbot::run(cmd);
		misc::getVelocityData();
	}	
	prediction::end();

	if (sendPacket)
		globals::realAngle = cmd->m_viewangles;
	else
		globals::fakeAngle = cmd->m_viewangles;

	return false;
}