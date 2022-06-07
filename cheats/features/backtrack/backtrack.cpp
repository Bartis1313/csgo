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

void Backtrack::update(int frame)
{
	// get time from every frame
	float correcttime = config.get<float>(vars.fBacktrackTick) / 1000.0f + extraTicks();

	if (frame != FRAME_RENDER_START)
		return;

	if (!game::localPlayer || !config.get<bool>(vars.bBacktrack) || !game::localPlayer->isAlive())
	{
		// basically reset all
		for (auto& el : m_records)
			el.clear();
		return;
	}

	constexpr auto isGoodEnt = [](Player_t* ent)
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
			//m_correct.at(i).reset();
			continue;
		}

		// purpose: nothing
		//bool correct = false;
		//if (m_correct.at(i).m_correctSimtime != entity->m_flSimulationTime() && m_correct.at(i).m_correctSimtime < entity->m_flSimulationTime())
		//{
		//	correct = true;
		//	if(!m_correct.at(i).m_correctSimtime)
		//		m_correct.at(i).m_correctSimtime = entity->m_flSimulationTime();

		//	//printf("correct %f game %f servertime %f\n", m_correct.at(i).m_correctSimtime, interfaces::globalVars->m_curtime, game::serverTime());

		//	m_correct.at(i).m_origin = entity->absOrigin();
		//}

		//if (correct)
		//{
		//	//printf("size %i\n", entity->m_CachedBoneData().m_size);

		//	m_correct.at(i).m_correctSimtime = entity->m_flSimulationTime();
		//	m_correct.at(i).m_setup = entity->setupBones(m_correct.at(i).m_matrix.data(), entity->m_CachedBoneData().m_size, // don't abuse setupbones with huge sizes
		//		BONE_USED_MASK, interfaces::globalVars->m_curtime);
		//}

		// if record at this index is filled and has exactly same simulation time, don't update it
		if (m_records.at(i).size() && (m_records.at(i).front().m_simtime == entity->m_flSimulationTime()))
			continue;

		if (!isValid(entity->m_flSimulationTime() /*m_correct.at(i).m_correctSimtime*/))
			continue;

		StoredRecord record = {};
		record.m_origin = entity->absOrigin();
		record.m_simtime = entity->m_flSimulationTime();
		if (!entity->setupBonesShort(record.m_matrix.data(), entity->m_CachedBoneData().m_size,
			BONE_USED_MASK, interfaces::globalVars->m_curtime))
			continue;
		record.m_head = record.m_matrix[8].origin();
		//record.m_origin = m_correct.at(i).m_origin;
		//record.m_simtime = m_correct.at(i).m_correctSimtime;
		//std::copy(m_correct.at(i).m_matrix.begin(), m_correct.at(i).m_matrix.end(), record.m_matrix);
		//record.m_head = record.m_matrix[8].origin();

		// fill them
		m_records.at(i).push_front(record);

		// when records are FULL and bigger than ticks we set in backtrack, then pop them
		while (m_records.at(i).size() > 3 && m_records.at(i).size() > static_cast<size_t>(TIME_TO_TICKS(correcttime)))
			m_records.at(i).pop_back();

		// if it's not valid then clean up everything on this record
		if (auto invalid = std::find_if(std::cbegin(m_records.at(i)), std::cend(m_records.at(i)), [this](const StoredRecord& rec)
			{
				return !isValid(rec.m_simtime);
			}); invalid != std::cend(m_records.at(i)))
			m_records.at(i).erase(invalid, std::cend(m_records.at(i)));
	}
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

	const auto aimPunch = game::localPlayer->getAimPunch();
	const auto myEye = game::localPlayer->getEyePos();

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		const auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!ent)
			continue;

		if (ent == game::localPlayer)
			continue;

		if (!ent->isAlive())
			continue;

		if (ent->isDormant())
			continue;

		if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		const auto& pos = ent->absOrigin();

		const auto fov = math::calcFovReal(myEye, pos, cmd->m_viewangles + aimPunch);
		if (fov < bestFov)
		{
			bestFov = fov;
			bestPlayer = ent;
			bestPlayerIdx = i;
		}
	}

	if (bestPlayer)
	{
		bestFov = 180.0f;

		for (int i = 0; i < m_records.at(bestPlayerIdx).size(); i++)
		{
			const auto& record = m_records.at(bestPlayerIdx).at(i);
			if (!isValid(record.m_simtime))
				continue;

			const auto fov = math::calcFovReal(myEye, record.m_head, cmd->m_viewangles + aimPunch);

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

void Backtrack::addLatency(INetChannel* netChannel, float latency)
{
	for (auto& el : m_sequences)
	{
		if (game::serverTime() - el._m_curtime >= latency)
		{
			netChannel->m_inReliableState = el.m_inReliableState;
			netChannel->m_inSequenceNr = el.m_sequenceNr;
			break;
		}
	}
}

void Backtrack::updateSequences()
{
	if (!game::isAvailable())
		return;

	if (!config.get<bool>(vars.bFakeLatency))
	{
		clearSequences();
		return;
	}

	auto network = interfaces::engine->getNameNetChannel();
	if (!network)
		return;

	if (m_lastSequence == 0)
		m_lastSequence = network->m_inSequenceNr;

	if (network->m_inSequenceNr > m_lastSequence)
	{
		m_lastSequence = network->m_inSequenceNr;
		m_sequences.emplace_front(
			SequenceRecord
			{
					network->m_inReliableState,
					network->m_outReliableState,
					network->m_inSequenceNr,
					game::serverTime()
			}
		);
	}

	if (m_sequences.size() > 2048)
		m_sequences.pop_back();
}

void Backtrack::clearSequences()
{
	if (!m_sequences.empty())
	{
		m_lastSequence = 0;
		m_sequences.clear();
	}
}
