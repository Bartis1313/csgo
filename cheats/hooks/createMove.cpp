#include "hooks.hpp"

#include "../../SDK/CUserCmd.hpp"
#include "../../SDK/Input.hpp"
#include "../../SDK/IClientEntityList.hpp"
#include "../../SDK/IVEngineClient.hpp"
#include "../../SDK/ClientClass.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/aimbot/aimbot.hpp"
#include "../features/prediction/prediction.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/misc/movement.hpp"
#include "../features/aimbot/triggerbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/misc/misc.hpp"
#include "../features/prediction/nadepred.hpp"
#include "../features/misc/freeLook.hpp"
#include "../features/misc/freeCam.hpp"

#include "../game.hpp"
#include "../globals.hpp"

#pragma warning(disable: 4409)

//bool __stdcall hooks::createMove::hooked(float inputFrame, CUserCmd* cmd)
//{
//	static auto orig = original;
//
//	game::localPlayer = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getLocalPlayer()));
//
//	if (!cmd || !cmd->m_commandNumber || !game::localPlayer)
//		return orig(inputFrame, cmd);
//
//	// thanks for reminding me https://github.com/Bartis1313/csgo/issues/4
//	if (orig(inputFrame, cmd))
//		interfaces::prediction->setLocalViewangles(cmd->m_viewangles);
//
//	return false;
//}

// to get the sendPacket correctly and no need to define it anywhere in headers
void __stdcall createMoveProxy(int sequence, float inputTime, bool active, bool& sendPacket)
{
	hooks::proxyCreateMove::original(sequence, inputTime, active);

	CUserCmd* cmd = interfaces::input->getUserCmd(0, sequence);
	if (!cmd || !cmd->m_commandNumber)
		return;

	CVerifiedUserCmd* verifiedCmd = interfaces::input->getVerifiedUserCmd(sequence);
	if (!verifiedCmd)
		return;

	Vector oldAngle = cmd->m_viewangles;

	// otherwise we moving
	if (freeCam.isInCam())
	{
		cmd->m_forwardmove = 0;
		cmd->m_sidemove = 0;
	}

	game::serverTime(cmd);
	movement.bunnyhop(cmd);
	movement.strafe(cmd);
	nadePred.createMove(cmd->m_buttons);
	backtrack.updateSequences();
	freeLook.createMove(cmd);

	prediction.update();
	prediction.addToPrediction(cmd, [=]()
		{
			backtrack.run(cmd);
			aimbot.run(cmd);
			aimbot.runRCS(cmd);
			triggerbot.run(cmd);
			misc.getVelocityData();
		});

	movement.fix(cmd, oldAngle);

	// don't get untrusted
	cmd->m_viewangles.normalize();
	cmd->m_viewangles.clamp();

	verifiedCmd->m_cmd = *cmd;
	verifiedCmd->m_crc = cmd->getChecksum();
}

// wrapper for function
__declspec(naked) void __stdcall hooks::proxyCreateMove::hooked(int sequence, float inputFrame, bool active)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx
		lea ecx, [esp]
		push ecx
		/*push dword ptr[active]
		push dword ptr[inputFrame]
		push dword ptr[sequence]*/
		push active
		push inputFrame
		push sequence
		call createMoveProxy
		pop ebx
		pop ebp
		retn 0xC
	}
}