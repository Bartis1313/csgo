#include "hooks.hpp"
#include "../game.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/prediction/prediction.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/misc/bunnyhop.hpp"
#include "../features/aimbot/triggerbot.hpp"
#include "../globals.hpp"
#include "../features/visuals/world.hpp"
#include "../features/misc/misc.hpp"

// might be removed, and move to proxy
bool __stdcall hooks::createMove::hooked(float inputFrame, CUserCmd* cmd)
{
	static auto orig = original;

	game::localPlayer = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getLocalPlayer()));

	if (!cmd || !cmd->m_commandNumber || !game::localPlayer)
		return orig(inputFrame, cmd);

	// thanks for reminding me https://github.com/Bartis1313/csgo/issues/4
	if (orig(inputFrame, cmd))
		interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	game::serverTime(cmd);
	bunnyhop::run(cmd);
	bunnyhop::strafe(cmd);

	// should be placed in proxy if you need packet
	prediction::start(cmd);
	{
		backtrack::run(cmd);
		legitbot::run(cmd);
		legitbot::runRCS(cmd);
		triggerbot::run(cmd);
		misc::getVelocityData();
	}
	prediction::end();

	// recent update killed it, nothing on stack

	return false;
}

// to get the sendPacket correctly and no need to define it anywhere in headers
void __stdcall createMoveProxy(int sequence, float inputTime, bool active, bool& sendPacket)
{
	hooks::proxyCreateMove::original(interfaces::client, 0, sequence, inputTime, active);

	CUserCmd* cmd = interfaces::input->getUserCmd(0, sequence);
	if (!cmd || !cmd->m_commandNumber)
		return;

	CVerifiedUserCmd* verifiedCmd = interfaces::input->getVerifiedUserCmd(sequence);
	if (!verifiedCmd)
		return;

	if (sendPacket)
	{
		// do anything that needs it, if you rely on prediction, then move prediction to the proxy hook
	}

	verifiedCmd->m_cmd = *cmd;
	verifiedCmd->m_crc = cmd->getChecksum();
}

// wrapper for function
__declspec(naked) void __fastcall hooks::proxyCreateMove::hooked(void*, int, int sequence, float inputTime, bool active)
{
	__asm
	{
		push ebp
		mov	ebc, esp
		push ebx
		push esp
		push dword ptr[active]
		push dword ptr[inputTime]
		push dword ptr[sequence]
		call createMoveProxy
		pop ebx
		pop ebp
		retn 0xC
	}
}