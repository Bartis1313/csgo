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
}