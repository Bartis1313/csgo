#include "hooks.hpp"

#include "../classes/createMove.hpp"
#include "../features/prediction/prediction.hpp"
#include "../features/misc/movement/movement.hpp"
#include "../features/misc/cameras/freeCam.hpp"
#include "../features/aimbot/cmdCache.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/Input.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>

hooks::createMove::value FASTCALL hooks::createMove::hooked(FAST_ARGS, float inputFrame, CUserCmd* cmd)
{
	original(thisptr, inputFrame, cmd);

	if (!cmd || !cmd->m_commandNumber || !game::localPlayer)
		return original(thisptr, inputFrame, cmd);

	// thanks for reminding me https://github.com/Bartis1313/csgo/issues/4
	if (original(thisptr, inputFrame, cmd))
		memory::interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	Vec3 oldAngle = cmd->m_viewangles;

	// otherwise we moving
	if (g_Freecam->isInCam())
	{
		cmd->m_buttons = 0;
		cmd->m_forwardmove = 0;
		cmd->m_sidemove = 0;
		cmd->m_upmove = 0;
	}

	CUserCmdCache::run(cmd);

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

	return false;
}