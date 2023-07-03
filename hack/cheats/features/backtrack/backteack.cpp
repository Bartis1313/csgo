#include "backtrack.hpp"

#include "../cache/cache.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/vars.hpp>
#include <SDK/Enums.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>

#include <cheats/hooks/createMove.hpp>
#include <cheats/hooks/frameStageNotify.hpp>

namespace
{
	struct BacktrackCMHandler : hooks::CreateMove
	{
		BacktrackCMHandler()
		{
			this->registerInit(backtrack::init);
			this->registerRunPrediction(backtrack::run);
		}
	} handlerCM;

	struct BacktrackStageHandler : hooks::FrameStageNotify
	{
		BacktrackStageHandler()
		{
			this->registerRun(backtrack::updater::run);
		}
	} handlerFSN;
}

namespace backtrack
{
	IConVar* cl_updaterate{ nullptr };
	IConVar* sv_maxupdaterate{ nullptr };
	IConVar* cl_interp{ nullptr };
	IConVar* cl_interp_ratio{ nullptr };
	IConVar* sv_client_min_interp_ratio{ nullptr };
	IConVar* sv_client_max_interp_ratio{ nullptr };
	IConVar* sv_maxunlag{ nullptr };
	float correctTime{ };

	float extraTicks();
}

void backtrack::init()
{
	cl_updaterate = memory::interfaces::cvar->findVar("cl_updaterate");
	sv_maxupdaterate = memory::interfaces::cvar->findVar("sv_maxupdaterate");

	cl_interp = memory::interfaces::cvar->findVar("cl_interp");
	cl_interp_ratio = memory::interfaces::cvar->findVar("cl_interp_ratio");
	sv_client_min_interp_ratio = memory::interfaces::cvar->findVar("sv_client_min_interp_ratio");
	sv_client_max_interp_ratio = memory::interfaces::cvar->findVar("sv_client_max_interp_ratio");
	sv_maxunlag = memory::interfaces::cvar->findVar("sv_maxunlag");
};

float backtrack::getLerp()
{
	float updateRate = sv_maxupdaterate ? sv_maxupdaterate->getFloat() : cl_updaterate->getFloat();
	float ratio = cl_interp_ratio->getFloat() == 0.0f ? 1.0f : cl_interp_ratio->getFloat();

	ratio = std::clamp(ratio, sv_client_min_interp_ratio->getFloat(), sv_client_max_interp_ratio->getFloat());
	return std::max(cl_interp->getFloat(), (ratio / updateRate));
}

bool backtrack::isValid(float simtime)
{
	const auto network = memory::interfaces::engine->getNameNetChannel();
	if (!network)
		return false;

	if (auto time = static_cast<int>(game::serverTime() - sv_maxunlag->getFloat()); simtime < time)
	{
		//printf("simtime precheck, sim %f lag sim %f\n", simtime, game::serverTime() - cvarsRatios.maxUnlag);
		return false;
	}

	const auto delta = std::clamp(network->getLatency(FLOW_OUTGOING) + network->getLatency(FLOW_INCOMING) + getLerp(),
		0.0f, sv_maxunlag->getFloat()) - (game::serverTime() - simtime);
	return std::abs(delta) <= 0.2f;
}

float backtrack::extraTicks()
{
	if (!vars::misc->fakeLatency->enabled)
		return 0.0f;

	return vars::misc->fakeLatency->amount / 1000.0f;
}

void backtrack::run(CUserCmd* cmd)
{
	if (!vars::backtrack->enabled)
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	if (!(cmd->m_buttons & IN_ATTACK))
		return;

	float bestFov = 180.0f;
	Player_t* bestPlayer = nullptr;
	int bestPlayerIdx = -1;
	int bestRecordIdx = -1;
	Vec3 bestPos = {};

	const auto aimPunch = game::localPlayer->getAimPunch();
	const auto myEye = game::localPlayer->getEyePos();

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		if (ent == game::localPlayer)
			continue;

		if (!ent->isAlive())
			continue;

		if (ent->isDormant())
			continue;

		if (!ent->isOtherTeam(game::localPlayer()))
			continue;

		const auto& pos = ent->absOrigin();

		const auto fov = math::calcFovReal(myEye, pos, cmd->m_viewangles + aimPunch);
		if (fov < bestFov)
		{
			bestFov = fov;
			bestPlayer = ent;
			bestPlayerIdx = idx;
			bestPos = pos;
		}
	}

	if (bestPlayer)
	{
		if (game::localPlayer->m_flFlashDuration() > 0.0f)
		{
			if (game::localPlayer->m_flFlashBangTime() >= vars::backtrack->flashLimit)
				return;
		}

		if (vars::backtrack->smoke && game::localPlayer->isViewInSmoke(bestPos))
			return;

		bestFov = 180.0f;

		for (size_t i = 0; const auto & el : records.at(bestPlayerIdx))
		{
			if (!isValid(el.simtime))
				continue;

			const auto fov = math::calcFovReal(myEye, el.head, cmd->m_viewangles + aimPunch);

			if (fov < bestFov)
			{
				bestFov = fov;
				bestRecordIdx = i;
			}

			i++;
		}
	}

	if (bestRecordIdx != -1)
	{
		const auto& record = records.at(bestPlayerIdx).at(bestRecordIdx);

		auto simTimeCorrected = record.simtime;
		if (auto deltaLerp = getLerp() - memory::interfaces::globalVars->m_intervalPerTick; deltaLerp > 0.0f)
			simTimeCorrected += memory::interfaces::globalVars->m_intervalPerTick - deltaLerp;

		cmd->m_tickcount = game::timeToTicks(simTimeCorrected + getLerp());
	}
}

void backtrack::updater::run(FrameStage stage)
{
	correctTime = vars::backtrack->time / 1000.0f + extraTicks();

	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	if (!vars::backtrack->enabled || !game::localPlayer->isAlive())
	{
		for (auto& el : records)
			el.clear();
		return;
	}

	constexpr auto isGoodEnt = [](Player_t* ent)
	{
		if (ent == game::localPlayer)
			return false;

		if (ent->isDormant())
			return false;

		if (!ent->isAlive())
			return false;

		if (!ent->isOtherTeam(game::localPlayer()))
			return false;

		return true;
	};

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		const auto ent = reinterpret_cast<Player_t*>(entity);
		const auto i = idx;

		if (!isGoodEnt(ent))
		{
			records.at(i).clear();
			continue;
		}

		if (records.at(i).size() && (records.at(i).front().simtime == ent->m_flSimulationTime()))
			continue;

		if (!isValid(ent->m_flSimulationTime() /*m_correct.at(i).m_correctSimtime*/))
			continue;

		StoredRecord record{ };
		record.origin = ent->absOrigin();
		record.simtime = ent->m_flSimulationTime();
		if (!ent->setupBonesShort(record.matrices.data(), ent->m_CachedBoneData().m_size,
			BONE_USED_MASK, memory::interfaces::globalVars->m_curtime))
			continue;
		record.head = record.matrices[8].origin();
		record.angle = ent->absAngles();

		records.at(i).push_front(record);

		while (records.at(i).size() > 3 && records.at(i).size() > static_cast<size_t>(game::timeToTicks(correctTime)))
			records.at(i).pop_back();

		const auto invalid = std::find_if(std::cbegin(records.at(i)), std::cend(records.at(i)), [](const StoredRecord& rec)
			{
				return !isValid(rec.simtime);
			});

		if (invalid != std::cend(records.at(i)))
			records.at(i).erase(invalid, std::cend(records.at(i)));
	}
}
