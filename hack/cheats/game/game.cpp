#include "game.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>

bool game::isAvailable()
{
	if (!localPlayer)
		return false;

	if (!memory::interfaces::engine->isConnected())
		return false;

	if (!memory::interfaces::engine->isInGame())
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
	return tick * memory::interfaces::globalVars->m_intervalPerTick;
}

void LocalPlayer::init()
{
	m_local = memory::localPlayer();
}

uint32_t game::timeToTicks(float time)
{
	return static_cast<uint32_t>(0.5f + time / memory::interfaces::globalVars->m_intervalPerTick);
}

float game::ticksToTime(uint32_t ticks)
{
	return memory::interfaces::globalVars->m_intervalPerTick * static_cast<float>(ticks);
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

#include <SDK/CHudChat.hpp>

bool game::isChatOpen()
{
	const auto chat = findHudElement<CHudChat*>("CCSGO_HudChat");
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
		if (name.find("ggrenade") != std::string::npos)
			return WEAPON_HEGRENADE;
		else
			return WEAPON_FLASHBANG;
	}
	case CSmokeGrenadeProjectile:
		return WEAPON_SMOKEGRENADE;
	case CMolotovProjectile:
	{
		if (name.find("molotov") != std::string::npos)
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
	memory::interfaces::engine->getViewAngles(ret);

	return ret;
}

#include <SDK/CGameMovement.hpp>
#include <SDK/IWeapon.hpp>

Vec3 game::getFixedPunch()
{
	static auto prevPunch = Vec3{};
	if (!game::localPlayer->getActiveWeapon())
		return game::localPlayer->m_aimPunchAngle();

	float time = game::localPlayer->getActiveWeapon()->m_flNextPrimaryAttack() - memory::interfaces::globalVars->m_curtime;
	int ticks = timeToTicks(time);

	const auto& punchNow = game::localPlayer->m_aimPunchAngle();

	if (ticks <= 0)
		prevPunch = punchNow;
	else
	{
		for (int i = 0; i < ticks; i++)
		{
			memory::interfaces::gameMovement->decayAimPunchAngle();
		}
	}

	auto& decayed = game::localPlayer->m_aimPunchAngle();
	game::localPlayer->m_aimPunchAngle() = punchNow;

	if (const auto info = game::localPlayer->getActiveWeapon()->getWpnInfo())
	{
		const float lerped = std::clamp(1.0f - (time / info->m_cycleTime), 0.0f, 1.0f);
		decayed = prevPunch.lerp(decayed, lerped);
	}

	return decayed;
}
