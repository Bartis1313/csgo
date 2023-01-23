#include "cfgWeapon.hpp"

#include <SDK/Enums.hpp>

WeaponList CfgWeapon::getWeaponByIndex(int weaponID)
{
	switch (weaponID)
	{
	case WEAPON_GLOCK:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_P250:
	case WEAPON_TEC9:
	case WEAPON_FIVESEVEN:
	case WEAPON_ELITE:
	case WEAPON_HKP2000:
		return WeaponList::WEAPON_PISTOL;
	case WEAPON_DEAGLE:
		return WeaponList::WEAPON_DEAGLE;
	case WEAPON_REVOLVER:
		return WeaponList::WEAPON_REVOLVER;
	case WEAPON_AK47:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_GALILAR:
	case WEAPON_FAMAS:
	case WEAPON_SG553:
	case WEAPON_AUG:
		return WeaponList::WEAPON_RIFFLE;
	case WEAPON_SSG08:
		return WeaponList::WEAPON_SCOUT;
	case WEAPON_AWP:
		return WeaponList::WEAPON_AWP;
	case WEAPON_SCAR20:
	case WEAPON_G3SG1:
		return WeaponList::WEAPON_AUTOSNIPER;
	case WEAPON_NOVA:
	case WEAPON_MAG7:
	case WEAPON_XM1014:
	case WEAPON_SAWEDOFF:
		return WeaponList::WEAPON_SHOTGUN;
	case WEAPON_NEGEV:
	case WEAPON_M249:
		return WeaponList::WEAPON_MG;
	case WEAPON_P90:
	case WEAPON_MAC10:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_MP5SD:
	case WEAPON_UMP45:
	case WEAPON_BIZON:
		return WeaponList::WEAPON_SMG;
	default:
		return WeaponList::WEAPON_UNKNOWN;
	}
}