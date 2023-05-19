#include "createMove.hpp"

#include "../features/prediction/prediction.hpp"
#include "../features/misc/movement/movement.hpp"
#include "../features/misc/cameras/freeCam.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/Input.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/IPrediction.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>

hooks::CreateMove::value hooks::CreateMove::hook(FAST_ARGS, float inputFrame, CUserCmd* cmd)
{
	original(thisptr, inputFrame, cmd);

	if (!cmd || !cmd->m_commandNumber || !game::localPlayer)
		return original(thisptr, inputFrame, cmd);

	// thanks for reminding me https://github.com/Bartis1313/csgo/issues/4
	if (original(thisptr, inputFrame, cmd))
		memory::interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	Vec3 oldAngle = cmd->m_viewangles;

	// otherwise we moving
	if (freecam::isInCam)
	{
		cmd->m_buttons = 0;
		cmd->m_forwardmove = 0;
		cmd->m_sidemove = 0;
		cmd->m_upmove = 0;
	}

	game::serverTime(cmd);

	Storage::runsPrePrediction.run(cmd);
	{
		prediction::begin(cmd);
		Storage::runsPrediction.run(cmd);
		prediction::end();
	}
	Storage::runsPostPrediction.run(cmd);

	movement::fixMovement(cmd, oldAngle);

	// don't get untrusted
	cmd->m_viewangles[0] = std::clamp(cmd->m_viewangles[0], -89.0f, 89.0f);
	cmd->m_viewangles[1] = std::clamp(cmd->m_viewangles[1], -180.0f, 180.0f);
	cmd->m_viewangles[2] = 0.0f;

	cmd->m_forwardmove = std::clamp(cmd->m_forwardmove, -450.0f, 450.0f);
	cmd->m_sidemove = std::clamp(cmd->m_sidemove, -450.0f, 450.0f);
	cmd->m_upmove = std::clamp(cmd->m_upmove, -320.0f, 320.0f);

	cmd->m_viewAnglesCopy = cmd->m_viewangles;
	cmd->m_buttonsCopy = cmd->m_buttons;

	return false;
}