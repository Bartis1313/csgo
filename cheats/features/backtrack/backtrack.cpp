#include "backtrack.hpp"

#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/vars.hpp"
#include "../../../SDK/Enums.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../game.hpp"
#include "../../../config/vars.hpp"

#include "../../../utilities/math/math.hpp"

static constexpr int TIME_TO_TICKS(float t)
{
	return static_cast<int>(0.5f + t / interfaces::globalVars->m_intervalPerTick);
}
//static constexpr int TICKS_TO_TIME(float t)
//{
//	return interfaces::globalVars->m_intervalPerTick * t;
//}

struct convars
{
	IConVar* updateRate;
	IConVar* maxUpdateRate;
	IConVar* minUpdateRate;
} static cvars;

struct convarRatios
{
	float interp;
	float interpRatio;
	float minInterpRatio;
	float maxInterpRatio;
	float maxUnlag;
} static cvarsRatios;

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
	// from my experience making it with game::serverTime() works slightly better
	auto delta = std::clamp(network->getLatency(FLOW_OUTGOING) + network->getLatency(FLOW_INCOMING) + getLerp(), 0.0f, cvarsRatios.maxUnlag) - (game::serverTime() - simtime);
	// this is not used, that's why this is commented, feel free to use it and config it yourself
	// auto limit = std::clamp(vars::iBacktrackTick, 0.0f, 2.0f);
	// if limit is used return looks like: return std::abs(delta) <= limit;
	return std::abs(delta) <= 0.2f;
}

float Backtrack::extraTicks() const
{
	auto network = interfaces::engine->getNameNetChannel();
	if (!network)
		return 0.0f;
	// extra ticks, due to latency you might store wrong ticks
	return std::clamp(network->getLatency(FLOW_INCOMING) - network->getLatency(FLOW_OUTGOING), 0.0f, 0.2f);
}

void Backtrack::update()
{
	if (!game::localPlayer || !config.get<bool>(vars.bBacktrack) || !game::localPlayer->isAlive())
	{
		// basically reset all
		for (auto& el : m_records)
			el.clear();
		return;
	}

	auto isGoodEnt = [](Player_t* ent)
	{
		if (!ent)
			return false;

		if (ent == game::localPlayer)
			return false;

		if (ent->isDormant())
			return false;

		if (!ent->isAlive())
			return false;

		if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			return false;

		return true;
	};

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!isGoodEnt(entity))
		{
			// don't add bunch of useless records
			m_records.at(i).clear();
			continue;
		}
		// if record at this index is filled and has exactly same simulation time, don't update it
		if (m_records.at(i).size() && (m_records.at(i).front().m_simtime == entity->m_flSimulationTime()))
			continue;

		StoredRecord record = {};
		// head will be used for calculations, from my testing it seemed to be better
		record.m_origin = entity->absOrigin();
		record.m_simtime = entity->m_flSimulationTime();
		record.m_head = entity->getBonePos(8);

		// setup bones for us, will be needed for basically get good matrix, can draw backtrack'ed models etc...
		entity->setupBones(record.m_matrix, MAX_BONES, BONE_USED_BY_HITBOX, interfaces::globalVars->m_curtime);

		// fill them
		m_records.at(i).push_front(record);

		// when records are FULL and bigger than ticks we set in backtrack, then pop them
		while (m_records.at(i).size() > 3 && m_records.at(i).size() > static_cast<size_t>(TIME_TO_TICKS(config.get<float>(vars.fBacktrackTick) / 1000.0f + extraTicks())))
			m_records.at(i).pop_back();

		// lambda check for valid time simulation
		// check every record with simtime
		const auto invalid = std::find_if(std::cbegin(m_records.at(i)), std::cend(m_records.at(i)), [this](const StoredRecord& rec)
			{
				return !isValid(rec.m_simtime);
			});

		// if it's not valid then clean up everything on this record
		if (invalid != std::cend(m_records.at(i)))
			m_records.at(i).erase(invalid, std::cend(m_records.at(i)));
	}
}

void Backtrack::run(CUserCmd* cmd)
{
	// this is same in working like an aimbot, but in here we don't set any angles
	// we set origin and that's it, and go through nodes of records
	// Maybe: some backtrack aimbot

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

	const auto aimPunch = game::localPlayer->getAimPunch();
	const auto myEye = game::localPlayer->getEyePos();

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		const auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!ent)
			continue;

		if (!ent->isAlive())
			continue;

		if (ent->isDormant())
			continue;

		if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		const auto& pos = ent->getBonePos(8);

		const auto fov = math::calcFov(myEye, pos, cmd->m_viewangles + aimPunch);
		if (fov < bestFov)
		{
			bestFov = fov;
			bestPlayer = ent;
			bestPlayerIdx = i;
		}
	}

	if (bestPlayer && bestPlayerIdx != -1)
	{
		if (m_records.at(bestPlayerIdx).size() <= 3)
			return;

		bestFov = 180.0f;

		for (int i = 0; i < m_records.at(bestPlayerIdx).size(); i++)
		{
			const auto& record = m_records.at(bestPlayerIdx).at(i);
			if (!isValid(record.m_simtime))
				continue;

			const auto fov = math::calcFov(myEye, record.m_head, cmd->m_viewangles + aimPunch);

			if (fov < bestFov)
			{
				bestFov = fov;
				bestRecordIdx = i;
			}
		}
	}

	if (bestRecordIdx != -1)
	{
		const auto& record = m_records.at(bestPlayerIdx).at(bestRecordIdx);
		cmd->m_tickcount = TIME_TO_TICKS(record.m_simtime);
	}
}