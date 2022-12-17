#include "prediction.hpp"

#include <SDK/CGameMovement.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <game/game.hpp>
#include <gamememory/memory.hpp>

void Prediction::init()
{
	m_predicionRandomSeed = memory::predictionSeed();
	m_data = memory::predictionData();
}

// 55 8B EC 83 E4 C0 83 EC 34 53 56 8B 75 08
void Prediction::start(CUserCmd* cmd)
{
	if (!game::localPlayer)
		return;

	update();

	*game::localPlayer->m_pCurrentCommand() = cmd;
	game::localPlayer->m_LastCmd() = *cmd;

	*m_predicionRandomSeed = cmd->m_randomSeed;
	*memory::predictedPlayer() = game::localPlayer();

	// backup pre-prediction
	cache = PredictionCache
	{
		.isInPrediction = memory::interfaces::prediction->m_inPrediction,
		.isFirstTimePredicted = memory::interfaces::prediction->m_firstTimePredicted,
		.curTime = memory::interfaces::globalVars->m_curtime,
		.frameTime = memory::interfaces::globalVars->m_frametime,
		.tick = game::localPlayer->m_nTickBase(),
	};

	const auto weapon = game::localPlayer()->getActiveWeapon();
	if (weapon && !weapon->isNonAimable())
	{
		cache.weaponPenalty = weapon->getInaccuracy();
		cache.recoilIndex = weapon->m_flRecoilIndex();
	}

	memory::interfaces::globalVars->m_curtime = game::ticksToTime(getCorrectTick(cmd));
	memory::interfaces::globalVars->m_frametime = (memory::interfaces::prediction->m_enginePaused/* || game::localPlayer->m_fFlags() & FL_FROZEN*/)
		? 0.0f : memory::interfaces::globalVars->m_intervalPerTick;
	memory::interfaces::globalVars->m_tickCount = getCorrectTick(cmd);
	memory::interfaces::prediction->m_inPrediction = true;
	memory::interfaces::prediction->m_firstTimePredicted = false;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/prediction.cpp#L1058
	cmd->m_buttons |= game::localPlayer->m_afButtonForced();
	cmd->m_buttons &= ~game::localPlayer->m_afButtonDisabled();

	memory::interfaces::gameMovement->startTrackPredictionErrors(game::localPlayer());

	if (const auto vehHandle = game::localPlayer->m_hVehicle(); !vehHandle && cmd->m_impulse)
		*game::localPlayer->m_nImpulse() = cmd->m_impulse;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/prediction.cpp#L1090
	auto buttons = game::localPlayer->m_nButtons();
	const auto cmdButtons = cmd->m_buttons;
	const auto buttonChanged = cmdButtons ^ *buttons;
	game::localPlayer()->m_afButtonLast() = *buttons;
	*buttons = cmdButtons;
	game::localPlayer->m_afButtonPressed() = buttonChanged & cmdButtons;
	game::localPlayer->m_afButtonPressed() = buttonChanged & (~cmdButtons);

	memory::interfaces::prediction->checkMovingGround(game::localPlayer(), memory::interfaces::globalVars->m_frametime);
	memory::interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	if (game::localPlayer->physicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		game::localPlayer->preThink();

	if (const auto tick = game::localPlayer->m_nNextThinkTick();
		tick > 0 && tick <= getCorrectTick(cmd))
	{
		game::localPlayer->m_nNextThinkTick() = TICK_NEVER_THINK;
		game::localPlayer->think();
	}

	memory::interfaces::moveHelper->setHost(game::localPlayer());
	memory::interfaces::prediction->setupMove(game::localPlayer(), cmd, memory::interfaces::moveHelper(), m_data);
	memory::interfaces::gameMovement->processMovement(game::localPlayer(), m_data);
	memory::interfaces::prediction->finishMove(game::localPlayer(), cmd, m_data);
	memory::interfaces::moveHelper->processImpacts();

	game::localPlayer->postThink();

	getCorrectTick() = cache.tick;

	if (weapon && !weapon->isNonAimable())
		weapon->updateAccuracyPenalty();

	memory::interfaces::prediction->m_inPrediction = cache.isInPrediction;
	memory::interfaces::prediction->m_firstTimePredicted = cache.isFirstTimePredicted;
}

void Prediction::end()
{
	if (!game::localPlayer)
		return;

	memory::interfaces::gameMovement->finishTrackPredictionErrors(game::localPlayer());
	memory::interfaces::moveHelper->setHost(nullptr);

	*m_predicionRandomSeed = -1;
	*memory::predictedPlayer() = nullptr;
	*game::localPlayer->m_pCurrentCommand() = nullptr;

	memory::interfaces::globalVars->m_curtime = cache.curTime;
	memory::interfaces::globalVars->m_frametime = cache.frameTime;
	memory::interfaces::globalVars->m_tickCount = cache.tick;

	if (const auto wpn = game::localPlayer->getActiveWeapon(); wpn && !wpn->isNonAimable())
	{
		wpn->m_fAccuracyPenalty() = cache.weaponPenalty;
		wpn->m_flRecoilIndex() = cache.recoilIndex;
	}

	memory::interfaces::gameMovement->reset();
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
	const static auto state = memory::interfaces::clientState;
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/engine/cl_pred.cpp#L64
	if (bool validframe = state->m_deltaTick > 0)
		memory::interfaces::prediction->update(state->m_deltaTick, validframe,
			state->m_lastCommandAck, state->m_lastOutGoingCommand + state->m_chockedCommands);
}

int& Prediction::getCorrectTick(CUserCmd* cmd)
{
	static int tick;
	static CUserCmd* lastCmd;

	if (cmd)
	{
		if (!lastCmd || lastCmd->m_predicted)
			tick = game::localPlayer->m_nTickBase();
		else
			tick++;
		lastCmd = cmd;
	}
	return tick;
}