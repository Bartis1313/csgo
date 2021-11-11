#include "backtrack.hpp"
#include "../../menu/vars.hpp"
// useless windefs
#undef min
#undef max

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

void backtrack::init()
{
	cvars.updateRate = interfaces::console->FindVar(XOR("cl_updaterate"));
	cvars.maxUpdateRate = interfaces::console->FindVar(XOR("sv_maxupdaterate"));
	cvars.minUpdateRate = interfaces::console->FindVar(XOR("sv_minupdaterate"));

	cvarsRatios.interp = interfaces::console->FindVar(XOR("cl_interp"))->getFloat();
	cvarsRatios.interpRatio = interfaces::console->FindVar(XOR("cl_interp_ratio"))->getFloat();
	cvarsRatios.minInterpRatio = interfaces::console->FindVar(XOR("sv_client_min_interp_ratio"))->getFloat();
	cvarsRatios.maxInterpRatio = interfaces::console->FindVar(XOR("sv_client_max_interp_ratio"))->getFloat();
	cvarsRatios.maxUnlag = interfaces::console->FindVar(XOR("sv_maxunlag"))->getFloat();
};

float backtrack::getLerp()
{
	// get the correct value from ratio cvars, this can be done by only this way
	auto ratio = std::clamp(cvarsRatios.interpRatio, cvarsRatios.minInterpRatio, cvarsRatios.maxInterpRatio);
	return std::max(cvarsRatios.interp, (ratio / ((cvars.maxUpdateRate) ? cvars.maxUpdateRate->getFloat() : cvars.updateRate->getFloat())));
}

bool backtrack::isValid(float simtime)
{
	auto network = interfaces::engine->getNameNetChannel();
	if (!network)
		return false;
	// from my experience making it with game::serverTime() works slightly better
	auto delta = std::clamp(network->getLatency(0) + network->getLatency(1) + getLerp(), 0.f, cvarsRatios.maxUnlag) - (game::serverTime() - simtime);
	// this is not used, that's why this is commented, feel free to use it and config it yourself
	// auto limit = std::clamp(vars::iBacktrackTick, 0.0f, 2.0f);
	// if limit is used return looks like: return std::abs(delta) <= limit;
	return std::abs(delta) <= 0.2f;
}

static float extraTicks()
{
	auto network = interfaces::engine->getNameNetChannel();
	if (!network)
		return 0.0f;
	// extra ticks, sometimes due to latency, also by this don't add lerp to cmd tick
	return std::clamp(network->getLatency(1) - network->getLatency(0), 0.f, 0.2f);
}

void backtrack::update()
{

	if (!game::localPlayer || !vars::bBacktrack || !game::localPlayer->isAlive())
	{
		// basically reset all
		for (auto& el : records)
			el.clear();
		return;
	}


	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));
		if (!entity || entity == game::localPlayer || entity->isDormant() || !entity->isAlive() || entity->m_iTeamNum() == game::localPlayer->m_iTeamNum())
		{
			// don't add bucch of useless records
			records.at(i).clear();
			continue;
		}
		// if record at this index is filled and has exactly same simulation time, don't update it
		if (records.at(i).size() && (records.at(i).front().simTime == entity->m_flSimulationTime()))
			continue;

		// this pvs fix is like bruh fix
		/**reinterpret_cast<Vector*>((uintptr_t)entity + 0xA0) = entity->absOrigin();
		*reinterpret_cast<int*>((uintptr_t)entity + 0xA30) = interfaces::globalVars->m_frameCount;
		*reinterpret_cast<int*>((uintptr_t)entity + 0xA28) = 0;*/

		StoredRecord record = {};
		// head will be used for calculations, from my testing it seemed to be better
		//record.head = entity->getHitboxPos(HITBOX_HEAD);
		// later on will be used for setting the abs of entity
		record.origin = entity->absOrigin();
		record.simTime = entity->m_flSimulationTime();
		record.head = entity->getBonePosition(8);

		// useless fix
		// TODO: rebuild bones
		// entity->invalidateBoneCache();

		// setup bones for us, will be needed for basically get good matrix, can draw backtrack'ed models etc...
		entity->setupBones(record.matrix, BONE_USED_BY_HITBOX, BONE_USED_MASK, interfaces::globalVars->m_curtime);

		// fill them
		records.at(i).push_front(record);

		// when records are FULL and bigger than ticks we set in backtrack, then pop them
		while (records.at(i).size() > 3 && records.at(i).size() > static_cast<size_t>(TIME_TO_TICKS(static_cast<float>(vars::iBacktrackTick / 1000.0f + extraTicks()))))
			records.at(i).pop_back();

		// lambda check for valid time simulation
		auto check = [](const StoredRecord& rec) { return !isValid(rec.simTime); };
		// check every record with simtime
		const auto invalid = std::find_if(std::cbegin(records.at(i)), std::cend(records.at(i)), check);

		// if it's not valid then clean up everything on this record
		if (invalid != std::cend(records.at(i)))
			records.at(i).erase(invalid, std::cend(records.at(i)));
	}
}

void backtrack::run(CUserCmd* cmd)
{
	// this is same in working like an aimbot, but in here we don't set any angles
	// we set origin and that's it, and go through nodes of records
	// Maybe: some backtrack aimbot

	if (!vars::bBacktrack)
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

		const auto& pos = ent->getBonePosition(8);

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
		if (records.at(bestPlayerIdx).size() <= 3)
			return;

		bestFov = 180.0f;

		// if ticks method used
		// auto ticksPassed = 0;
		for (int i = 0; i < records.at(bestPlayerIdx).size(); /* && ticksPassed < vars::iBacktrackTick*/ i++)
		{
			// ticksPassed++;
			const auto& record = records.at(bestPlayerIdx).at(i);
			if (!isValid(record.simTime))
				continue;

			const auto fov = math::calcFov(myEye, record.head, cmd->m_viewangles + aimPunch);

			if (fov < bestFov)
			{
				bestFov = fov;
				bestRecordIdx = i;
			}
		}
	}

	if (bestRecordIdx != -1)
	{
		const auto& record = records.at(bestPlayerIdx).at(bestRecordIdx);
		cmd->m_tickcount = TIME_TO_TICKS(record.simTime);
	}
}