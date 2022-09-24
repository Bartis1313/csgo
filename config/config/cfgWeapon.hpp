#pragma once

#include "enums.hpp"

#include <array>

enum class WeaponList
{
	WEAPON_RIFFLE,
	WEAPON_SMG,
	WEAPON_MG,
	WEAPON_PISTOL,
	WEAPON_DEAGLE,
	WEAPON_REVOLVER,
	WEAPON_SCOUT,
	WEAPON_AWP,
	WEAPON_AUTOSNIPER,
	WEAPON_SHOTGUN,
	WEAPON_UNKNOWN,
	LIST_SIZE
};

class CfgWeapon
{
public:
	constexpr CfgWeapon()
	{
		m_aimEnabled = false;
		m_fov = 5.0f;
		m_smooth = 1.0f;
		m_methodAim = static_cast<int>(AimbotMethod::CROSSHAIR);
		m_aimSelection = static_cast<int>(AimbotID::NEAREST);
		m_RcsEnabled = false;
		m_aimDelayEnabled = false;
		m_aimDelay = 0.0f;
		m_aimbacktrack = false;
		m_RcsX = 50.0f;
		m_RcsY = 50.0f;
		m_TriggerEnabled = false;
		m_TriggerDelay = 0.0f;
		m_smokeCheck = true;
		m_flashAlphaLimit = 120.0f;
	}

	bool m_aimEnabled;
	float m_fov;
	float m_smooth;
	int m_methodAim;
	int m_aimSelection;
	bool m_aimDelayEnabled;
	float m_aimDelay;
	bool m_aimbacktrack;
	bool m_RcsEnabled;
	float m_RcsX;
	float m_RcsY;
	bool m_TriggerEnabled;
	float m_TriggerDelay;
	bool m_smokeCheck;
	float m_flashAlphaLimit;

	static WeaponList getWeaponByIndex(int weaponID);
};