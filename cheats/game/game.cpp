#include "game.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>

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
	m_local = g_Memory.m_localPlayer();
}

uint32_t game::timeToTicks(float time)
{
	return static_cast<uint32_t>(0.5f + time / interfaces::globalVars->m_intervalPerTick);
}

float game::ticksToTime(uint32_t ticks)
{
	return interfaces::globalVars->m_intervalPerTick * static_cast<float>(ticks);
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/183347-bomb-damage-indicator.html
float game::scaleDamageArmor(float dmg, const float armor)
{
	if (armor > 0.0f)
	{
		float newDmg = dmg * 0.5f;
		float scaledArmor = (dmg - newDmg) * 0.5f;

		if (scaledArmor > armor)
		{
			scaledArmor = armor * (1.0f / 0.5f);
			newDmg = dmg - scaledArmor;
		}
		dmg = newDmg;
	}

	return dmg;
}

uintptr_t* game::findHudElement(const std::string_view name)
{
	return g_Memory.m_hudfindElement()(g_Memory.m_csgoHud(), name.data());
}

#include <SDK/CHudChat.hpp>

bool game::isChatOpen()
{
	const auto chat = reinterpret_cast<CHudChat*>(findHudElement(XOR("CCSGO_HudChat")));
	return chat->m_isOpen;
}

float game::getScaledFont(const Vec3& source, const Vec3& destination, const float division, const float min, const float max)
{
	const float dist = source.distTo(destination);
	const float fontSize = std::clamp(division / (dist / division), min, max);
	return fontSize;
}

#include <SDK/CStudioHdr.hpp>

WeaponIndex game::getNadeByClass(int idx, Studiohdr_t* studio)
{
	switch (std::string_view name = studio->m_name; idx)
	{
	case CBaseCSGrenadeProjectile:
	{
		if (name.find(XOR("ggrenade")) != std::string::npos)
			return WEAPON_HEGRENADE;
		else
			return WEAPON_FLASHBANG;
	}
	case CSmokeGrenadeProjectile:
		return WEAPON_SMOKEGRENADE;
	case CMolotovProjectile:
	{
		if (name.find(XOR("molotov")) != std::string::npos)
			return WEAPON_MOLOTOV;
		else
			return WEAPON_INCGRENADE;
	}
	case CDecoyProjectile:
		return WEAPON_DECOY;
	default:
		break;
	}

	return WEAPON_NONE;
}

Vec3 game::getViewAngles()
{
	Vec3 ret;
	interfaces::engine->getViewAngles(ret);

	return ret;
}
