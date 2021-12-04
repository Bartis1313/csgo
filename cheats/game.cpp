#include "game.hpp"

namespace game
{
	bool isAvailable()
	{
		if (!localPlayer)
			return false;
	
		if (!interfaces::engine->isConnected())
			return false;
	
		if (!interfaces::engine->isInGame())
			return false;
	
		return true;
	}

	float serverTime(CUserCmd* cmd)
	{
		static int tick;
		static CUserCmd* lastCmd;

		if (cmd)
		{
			if (!lastCmd || lastCmd->m_predicted)
				tick = localPlayer->m_nTickBase();
			else
				tick++;
			lastCmd = cmd;
		}
		return tick * interfaces::globalVars->m_intervalPerTick;
	}

	int getLocalKills()
	{
		if (!localPlayer)
			return 0;
		const static auto res = *interfaces::resource;
		if (res)
		{
			const static auto kills = res->getKills(localPlayer->getIndex());
			return kills;
		}
		return 0;
	}
	int getLocalDeaths()
	{
		if (!localPlayer)
			return 0;
		const static auto res = *interfaces::resource;
		if (res)
		{
			const static auto deaths = res->getDeaths(localPlayer->getIndex());
			return deaths;
		}
		return 0;
	}

	int getLocalPing()
	{
		if (!localPlayer)
			return 0;
		const static auto res = *interfaces::resource;
		if (res)
		{
			const static auto ping = res->getPing(localPlayer->getIndex());
			return ping;
		}
		return 0;
	}
}