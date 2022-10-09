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
};

class CfgWeapon
{
public:
	bool enabled = false;
	float fov = 5.0f;
	float smooth = 0.5f;
	int smoothMode = static_cast<int>(SmoothMode::LINEAR);
	bool useExtraSpeed = false;
	bool curveAim = false;
	float curveX = 0.5f;
	float curveY = 0.3f;
	int methodAim = static_cast<int>(AimbotMethod::CROSSHAIR);;
	int aimSelection = static_cast<int>(AimbotHitboxes::NEAREST);
	bool aimDelay = false;
	float aimDelayVal = 0.0f;
	bool aimBacktrack = false;
	bool rcs = false;
	float rcsX = 0.5f;
	float rcsY = 0.5f;
	bool triggerbot = false;
	float triggerbotDelay = 150.0f;
	bool smokeCheck = true;
	float flashLimit = 120.0f;

	static WeaponList getWeaponByIndex(int weaponID);
};