#include "prediction.hpp"
#include "../../game.hpp"

// some globals
namespace
{
	static float curTime;
	static float frameTime;
	static uintptr_t* predicionRandomSeed;
	CMoveData data = {};
}

void prediction::start(CUserCmd* cmd)
{
	if (!game::localPlayer)
		return;

	// every frame this must be called, on end just reset
	if(!predicionRandomSeed)
		predicionRandomSeed = *reinterpret_cast<uintptr_t**>(utilities::patternScan(CLIENT_DLL, PREDICTIONRANDOMSEED) + 2);

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

void prediction::end()
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