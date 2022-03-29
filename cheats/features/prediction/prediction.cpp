#include "prediction.hpp"

#include "../../../SDK/CGameMovement.hpp"
#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/CGlobalVars.hpp"

#include "../../game.hpp"

void Prediction::start(CUserCmd* cmd)
{
	if (!game::localPlayer)
		return;

	// init once
	static auto bOnce = [this]()
	{
		predicionRandomSeed = *reinterpret_cast<uintptr_t**>(utilities::patternScan(CLIENT_DLL, PREDICTIONRANDOMSEED) + 0x2);
		return true;
	} ();

	// make it as a unique prediction, like md5 unique hash iirc or smth
	*predicionRandomSeed = cmd->m_randomSeed & 0x7FFFFFFF;

	// get current times to globals
	curTime = interfaces::globalVars->m_curtime;
	frameTime = interfaces::globalVars->m_frametime;

	// store not current, but valid ticks/times to the game
	interfaces::globalVars->m_curtime = game::serverTime(cmd);
	interfaces::globalVars->m_frametime = interfaces::globalVars->m_intervalPerTick;

	// use what SDK has given to us
	interfaces::gameMovement->startTrackPredictionErrors(game::localPlayer);

	// set temp data
	memset(&data, 0, sizeof(data));
	// again use what SDK has given to us
	interfaces::moveHelper->setHost(game::localPlayer);
	interfaces::prediction->setupMove(game::localPlayer, cmd, interfaces::moveHelper, &data);
	interfaces::gameMovement->processMovement(game::localPlayer, &data);
	interfaces::prediction->finishMove(game::localPlayer, cmd, &data);
}

void Prediction::end()
{
	if (!game::localPlayer)
		return;

	// just resets here, so we have valid prediction every frame

	interfaces::gameMovement->finishTrackPredictionErrors(game::localPlayer);
	interfaces::moveHelper->setHost(nullptr);

	*predicionRandomSeed = -1;

	interfaces::globalVars->m_curtime = curTime;
	interfaces::globalVars->m_frametime = frameTime;
}

void Prediction::addToPrediction(CUserCmd* cmd, const std::function<void()>& fun)
{
	start(cmd);
	fun();
	end();
}