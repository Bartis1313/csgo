#include "game.hpp"

#include "../SDK/IVEngineClient.hpp"
#include "../SDK/CUserCmd.hpp"
#include "../SDK/CGlobalVars.hpp"
#include "../SDK/interfaces/interfaces.hpp"
#include "../utilities/utilities.hpp"
#include "../SDK/structs/IDXandPaterrns.hpp"

bool game::isAvailable()
{
	if (!localPlayer)
		return false;

	if (!interfaces::engine->isConnected())
		return false;

	if (!interfaces::engine->isInGame())
		return false;

	return true;
}

float game::serverTime(CUserCmd* cmd)
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

void LocalPlayer::init()
{
	m_local = *reinterpret_cast<Player_t***>(utilities::patternScan(CLIENT_DLL, LOCAL_PLAYER, 0x2));
}