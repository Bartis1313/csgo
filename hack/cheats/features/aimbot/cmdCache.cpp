#include "cmdCache.hpp"

void CUserCmdCache::run(CUserCmd* cmd)
{
	m_cache = cmd;
}

#include <config/cfgWeapon.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>

std::optional<CfgWeapon> CUserCmdCache::getWeaponConfig()
{
	if(!game::isAvailable())
		return std::nullopt;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return std::nullopt;

	const auto weaponCfg = CfgWeapon::getWeaponByIndex(weapon->m_iItemDefinitionIndex());

	if (weaponCfg == WeaponList::WEAPON_UNKNOWN)
		return std::nullopt;

	return vars::aim->weapons.at(static_cast<size_t>(weaponCfg));
}