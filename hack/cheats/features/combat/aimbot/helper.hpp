#pragma once

#include <config/cfgWeapon.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>
#include <menu/GUI-ImGui/menu.hpp>

#include <optional>

namespace configWeapon
{
	inline std::optional<CfgWeapon> get()
	{
		if (ImGuiMenu::active)
			return std::nullopt;

		if (!game::localPlayer)
			return std::nullopt;

		const auto weapon = game::localPlayer->getActiveWeapon();
		if (!weapon)
			return std::nullopt;

		const auto weaponCfg = CfgWeapon::getWeaponByIndex(weapon->m_iItemDefinitionIndex());

		if (weaponCfg == WeaponList::WEAPON_UNKNOWN)
			return std::nullopt;

		return vars::aim->weapons.at(static_cast<size_t>(weaponCfg));
	}
}