#include "hooks.hpp"

#include "../classes/createMove.hpp"
#include "../features/prediction/prediction.hpp"
#include "../features/misc/movement/movement.hpp"
#include "../features/misc/cameras/freeCam.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/Input.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>

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
void FASTCALL createMoveProxy(FAST_ARGS, int sequence, float inputTime, bool active, bool& sendPacket)
{
	hooks::proxyCreateMove::original(thisptr, sequence, inputTime, active);

	CUserCmd* cmd = memory::interfaces::input->getUserCmd(0, sequence);
	if (!cmd || !cmd->m_commandNumber)
		return;

	CVerifiedUserCmd* verifiedCmd = memory::interfaces::input->getVerifiedUserCmd(sequence);
	if (!verifiedCmd)
		return;

	Vec3 oldAngle = cmd->m_viewangles;

	// otherwise we moving
	if (g_Freecam->isInCam())
	{
		cmd->m_buttons = 0;
		cmd->m_forwardmove = 0;
		cmd->m_sidemove = 0;
		cmd->m_upmove = 0;
	}

	game::serverTime(cmd);
	CreateMovePrePredictionType::runAll(cmd);

	g_Prediction->addToPrediction(cmd, [=]()
		{
			CreateMoveInPredictionType::runAll(cmd);
		});

	CreateMovePostPredictionType::runAll(cmd);

	MovementFix::run(cmd, oldAngle);

	// don't get untrusted
	cmd->m_viewangles.normalize();
	cmd->m_viewangles.clamp();

	verifiedCmd->m_cmd = *cmd;
	verifiedCmd->m_crc = cmd->getChecksum();
}

// wrapper for function
__declspec(naked) void __fastcall hooks::proxyCreateMove::hooked(FAST_ARGS, int sequence, float inputFrame, bool active)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx
		push esp
		push active
		push inputFrame
		push sequence
		call createMoveProxy
		pop ebx
		pop ebp
		retn 0xC
	}
}