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
#include <game/game.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>

void Backtrack::init()
{
	cvars.updateRate = interfaces::cvar->findVar(XOR("cl_updaterate"));
	cvars.maxUpdateRate = interfaces::cvar->findVar(XOR("sv_maxupdaterate"));
	cvars.minUpdateRate = interfaces::cvar->findVar(XOR("sv_minupdaterate"));

	cvarsRatios.interp = interfaces::cvar->findVar(XOR("cl_interp"))->getFloat();
	cvarsRatios.interpRatio = interfaces::cvar->findVar(XOR("cl_interp_ratio"))->getFloat();
	cvarsRatios.minInterpRatio = interfaces::cvar->findVar(XOR("sv_client_min_interp_ratio"))->getFloat();
	cvarsRatios.maxInterpRatio = interfaces::cvar->findVar(XOR("sv_client_max_interp_ratio"))->getFloat();
	cvarsRatios.maxUnlag = interfaces::cvar->findVar(XOR("sv_maxunlag"))->getFloat();
};

float Backtrack::getLerp() const
{
	// get the correct value from ratio cvars, this can be done by only this way
	auto ratio = std::clamp(cvarsRatios.interpRatio, cvarsRatios.minInterpRatio, cvarsRatios.maxInterpRatio);
	return std::max(cvarsRatios.interp, (ratio / ((cvars.maxUpdateRate) ? cvars.maxUpdateRate->getFloat() : cvars.updateRate->getFloat())));
}

bool Backtrack::isValid(float simtime) const
{
	auto network = interfaces::engine->getNameNetChannel();
	if (!network)
		return false;

	if (auto time = static_cast<int>(game::serverTime() - cvarsRatios.maxUnlag); simtime < time)
	{
		//printf("simtime precheck, sim %f lag sim %f\n", simtime, game::serverTime() - cvarsRatios.maxUnlag);
		return false;
	}

	auto delta = std::clamp(network->getLatency(FLOW_OUTGOING) + network->getLatency(FLOW_INCOMING) + getLerp(), 0.0f, cvarsRatios.maxUnlag) - (game::serverTime() - simtime);
	return std::abs(delta) <= 0.2f;
}

float Backtrack::extraTicks() const
{
	if (!config.get<bool>(vars.bFakeLatency))
		return 0.0f;

	return config.get<float>(vars.fFakeLatency) / 1000.0f;
}

void Backtrack::run(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bBacktrack))
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
	Vector bestPos = {};

	const auto aimPunch = game::localPlayer->getAimPunch();
	const auto myEye = game::localPlayer->getEyePos();

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::PLAYER))
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
			if (game::localPlayer->m_flFlashBangTime() >= config.get<float>(vars.fBacktrackFlashStart))
				return;
		}

		if (config.get<bool>(vars.bBacktrackSmoke) && game::localPlayer->isViewInSmoke(bestPos))
			return;

		bestFov = 180.0f;

		for (size_t i = 0; const auto & el : m_records.at(bestPlayerIdx))
		{
			if (!isValid(el.m_simtime))
				continue;

			const auto fov = math::calcFovReal(myEye, el.m_head, cmd->m_viewangles + aimPunch);

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
		const auto& record = m_records.at(bestPlayerIdx).at(bestRecordIdx);
		cmd->m_tickcount = game::timeToTicks(record.m_simtime);
	}
}

void BackTrackUpdater::init()
{

}

void BackTrackUpdater::run(int frame)
{
	// get time from every frame
	float correcttime = config.get<float>(vars.fBacktrackTick) / 1000.0f + g_Backtrack.extraTicks();

	if (frame != FRAME_RENDER_START)
		return;

	auto& records = g_Backtrack.getAllRecords();

	if (!game::localPlayer || !config.get<bool>(vars.bBacktrack) || !game::localPlayer->isAlive())
	{
		// basically reset all
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

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		auto i = idx;

		if (!isGoodEnt(ent))
		{
			// don't add bunch of useless records
			records.at(i).clear();
			continue;
		}

		// if record at this index is filled and has exactly same simulation time, don't update it
		if (records.at(i).size() && (records.at(i).front().m_simtime == ent->m_flSimulationTime()))
			continue;

		if (!g_Backtrack.isValid(ent->m_flSimulationTime() /*m_correct.at(i).m_correctSimtime*/))
			continue;

		Backtrack::StoredRecord record = {};
		record.m_origin = ent->absOrigin();
		record.m_simtime = ent->m_flSimulationTime();
		if (!ent->setupBonesShort(record.m_matrix.data(), ent->m_CachedBoneData().m_size,
			BONE_USED_MASK, interfaces::globalVars->m_curtime))
			continue;
		record.m_head = record.m_matrix[8].origin();

		// fill them
		records.at(i).push_front(record);

		// when records are FULL and bigger than ticks we set in backtrack, then pop them
		while (records.at(i).size() > 3 && records.at(i).size() > static_cast<size_t>(game::timeToTicks(correcttime)))
			records.at(i).pop_back();

		auto invalid = std::find_if(std::cbegin(records.at(i)), std::cend(records.at(i)), [this](const Backtrack::StoredRecord& rec)
			{
				return !g_Backtrack.isValid(rec.m_simtime);
			});
		// if it's not valid then clean up everything on this record
		if (invalid != std::cend(records.at(i)))
			records.at(i).erase(invalid, std::cend(records.at(i)));

		i++;
	}
}
