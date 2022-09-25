#include "prediction.hpp"

#include <SDK/CGameMovement.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <gamememory/memory.hpp>

void Prediction::init()
{
	m_predicionRandomSeed = g_Memory.m_predictionSeed();
	m_data = g_Memory.m_predictionData();
}

void Prediction::start(CUserCmd* cmd)
{
	if (!game::localPlayer)
		return;

	// make it as a unique prediction, md5 random
	*m_predicionRandomSeed = cmd->m_randomSeed & 0x7FFFFFFF;

	// get current times to globals
	m_curTime = interfaces::globalVars->m_curtime;
	m_frameTime = interfaces::globalVars->m_frametime;

	// store not current, but valid times to the game
	interfaces::globalVars->m_curtime = game::serverTime(cmd);
	interfaces::globalVars->m_frametime = game::localPlayer->m_fFlags() & FL_FROZEN ? 0.0f : interfaces::globalVars->m_intervalPerTick;

	// use what SDK has given to us
	interfaces::gameMovement->startTrackPredictionErrors(game::localPlayer());

	// again use what SDK has given to us
	interfaces::moveHelper->setHost(game::localPlayer());
	interfaces::prediction->setupMove(game::localPlayer(), cmd, interfaces::moveHelper, m_data);
	interfaces::gameMovement->processMovement(game::localPlayer(), m_data);
	interfaces::prediction->finishMove(game::localPlayer(), cmd, m_data);
}

void Prediction::end()
{
	if (!game::localPlayer)
		return;

	// just resets here, so we have valid prediction every tick

	interfaces::gameMovement->finishTrackPredictionErrors(game::localPlayer());
	interfaces::moveHelper->setHost(nullptr);

	*m_predicionRandomSeed = -1;

	interfaces::globalVars->m_curtime = m_curTime;
	interfaces::globalVars->m_frametime = m_frameTime;
}

void Prediction::addToPrediction(CUserCmd* cmd, const std::function<void()>& fun)
{
	start(cmd);
	fun();
	end();
}

#include <SDK/IClientState.hpp>

void Prediction::update()
{
	const static auto state = interfaces::clientState;
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/engine/cl_pred.cpp#L64
	if (bool validframe = state->m_deltaTick > 0)
		interfaces::prediction->update(state->m_deltaTick, validframe,
			state->m_lastCommandAck, state->m_lastOutGoingCommand + state->m_chockedCommands);
}