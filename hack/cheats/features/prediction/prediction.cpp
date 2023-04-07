#include "prediction.hpp"

#include <SDK/CGameMovement.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/CPredictionCopy.hpp>
#include <SDK/IClientState.hpp>
#include <SDK/structs/Entity.hpp>
#include <cheats/game/game.hpp>
#include <gamememory/memory.hpp>

#include <mutex>

void Prediction::init()
{
	m_predicionRandomSeed = memory::predictionSeed();
	m_data = memory::predictionData();
	m_player = memory::predictedPlayer();
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
	*m_player = game::localPlayer();

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

	memory::interfaces::globalVars->m_curtime = game::ticksToTime(game::localPlayer->m_nTickBase());
	memory::interfaces::globalVars->m_frametime = (memory::interfaces::prediction->m_enginePaused/* || game::localPlayer->m_fFlags() & FL_FROZEN*/)
		? 0.0f : memory::interfaces::globalVars->m_intervalPerTick;
	memory::interfaces::globalVars->m_tickCount = game::localPlayer->m_nTickBase();
	memory::interfaces::prediction->m_inPrediction = true;
	memory::interfaces::prediction->m_firstTimePredicted = false;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/prediction.cpp#L1058
	cmd->m_buttons |= game::localPlayer->m_afButtonForced();
	cmd->m_buttons &= ~game::localPlayer->m_afButtonDisabled();

	memory::interfaces::gameMovement->startTrackPredictionErrors(game::localPlayer());

	if (cmd->m_weaponSelect != 0)
	{
		// >=0 < 0x2000
		const auto weaponFromHandle = reinterpret_cast<Weapon_t*>(memory::interfaces::entList->getClientEntity(cmd->m_weaponSelect));
		if (weaponFromHandle)
		{
			const auto data = memory::interfaces::weaponInterface->getWpnData(weaponFromHandle->m_iItemDefinitionIndex());
			if (data)
			{
				game::localPlayer->selectItem(data->m_WeaponName, cmd->m_weaponSubtype);
			}
		}
	}

	Entity_t* veh = nullptr;
	if (const auto vehHandle = game::localPlayer->m_hVehicle(); vehHandle.isValid()) // 0xFFFFFFFF
		veh = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientFromHandle(vehHandle));

	if (cmd->m_impulse && (!veh || game::localPlayer->usingStandardWeaponsInVehicle()))
		*game::localPlayer->m_nImpulse() = cmd->m_impulse;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/prediction.cpp#L1090
	const auto cmdButtons = cmd->m_buttons;
	const auto localButtons = *game::localPlayer->m_nButtons();
	const auto buttonChanged = cmdButtons ^ localButtons;
	game::localPlayer()->m_afButtonLast() = localButtons;
	*game::localPlayer->m_nButtons() = cmdButtons;
	game::localPlayer->m_afButtonPressed() = buttonChanged & cmdButtons;
	game::localPlayer->m_afButtonReleased() = buttonChanged & (~cmdButtons);

	memory::interfaces::prediction->checkMovingGround(game::localPlayer(), memory::interfaces::globalVars->m_frametime);
	memory::interfaces::prediction->setLocalViewangles(cmd->m_viewangles);

	if (game::localPlayer->physicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		game::localPlayer->preThink();

	if (auto tick = game::localPlayer->m_nNextThinkTick();
		*tick > 0 && *tick <= game::localPlayer->m_nTickBase())
	{
		*tick = TICK_NEVER_THINK;
		game::localPlayer->runThink();
	}

	memory::interfaces::moveHelper->setHost(game::localPlayer());
	memory::interfaces::prediction->setupMove(game::localPlayer(), cmd, memory::interfaces::moveHelper(), m_data);
	if (!veh)
		memory::interfaces::gameMovement->processMovement(game::localPlayer(), m_data);
	else
		game::localPlayer->processMovement(veh, game::localPlayer(), m_data);
	memory::interfaces::prediction->finishMove(game::localPlayer(), cmd, m_data);
	memory::interfaces::moveHelper->processImpacts();
	//game::localPlayer->m_vecAbsVelocity() = m_data->m_velocity;
	//game::localPlayer->setAbsOrigin(game::localPlayer->m_vecNetworkOrigin());
	// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/prediction.cpp#L1513
	//game::localPlayer->m_iEFlags() &= ~(EFL_DIRTY_ABSTRANSFORM | EFL_DIRTY_ABSVELOCITY);

	game::localPlayer->postThink();

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
	*m_player = nullptr;
	*game::localPlayer->m_pCurrentCommand() = nullptr;

	memory::interfaces::globalVars->m_curtime = cache.curTime;
	memory::interfaces::globalVars->m_frametime = cache.frameTime;
	memory::interfaces::globalVars->m_tickCount = cache.tick;

	if (memory::interfaces::globalVars->m_frametime > 0.0f)
		++game::localPlayer->m_nTickBase();

	if (const auto wpn = game::localPlayer->getActiveWeapon(); wpn && !wpn->isNonAimable())
	{
		wpn->m_fAccuracyPenalty() = cache.weaponPenalty;
		wpn->m_flRecoilIndex() = cache.recoilIndex;
	}

	memory::interfaces::gameMovement->reset();

	//game::localPlayer->restoreData("Prediction::postEnd", 1313, PC_EVERYTHING);
}

void Prediction::addToPrediction(CUserCmd* cmd, const std::function<void()>& fun)
{
	start(cmd);
	fun();
	end();
}

void Prediction::patchDatamap()
{
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/564094-guwop-prediction-datamap-variable-fix.html
	const auto map = game::localPlayer->getPredictionDataMap();
	static std::unique_ptr<Typedescription_t[]> data(new Typedescription_t[map->m_dataFields + 1]);
	std::memcpy(data.get(), map->m_dataDescription, map->m_dataFields * sizeof(Typedescription_t));

	Typedescription_t velmod = {};

	velmod.m_type = FIELD_FLOAT;
	velmod.m_name = "m_flVelocityModifier";
	velmod.m_offset[TD_OFFSET_NORMAL] = netvarMan.getNetvar("DT_CSPlayer", "m_flVelocityModifier");
	//int copiedPackedSize = map->m_packedSize;
	//copiedPackedSize = ALIGN_VALUE(copiedPackedSize, sizeof(float));
	//velmod.m_offset[TD_OFFSET_PACKED] = copiedPackedSize;
	velmod.m_size = 1;
	velmod.m_flags = FTYPEDESC_INSENDTABLE;
	velmod.m_sizeInBytes = sizeof(float);
	velmod.fieldTolerance = 0.1f;

	std::memcpy(&data.get()[map->m_dataFields], &velmod, sizeof(Typedescription_t));

	map->m_dataDescription = data.get();
	map->m_dataFields += 1;
	// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/shared/predictioncopy.cpp#L1556
	map->m_optimizedDataMap = nullptr; // Forcing the game to recompute flattened chains.

	//memory::interfaces::prediction->shutdownPredictables();
	//memory::interfaces::prediction->reinitPredictables();
}

void Prediction::update()
{
	const static auto state = memory::interfaces::clientState;
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/engine/cl_pred.cpp#L64
	if (bool validframe = state->m_deltaTick > 0)
		memory::interfaces::prediction->update(state->m_deltaTick, validframe,
			state->m_lastCommandAck, state->m_lastOutGoingCommand + state->m_chockedCommands);

	//game::localPlayer->saveData("Prediction::preUpdate", 1313, PC_EVERYTHING);
}