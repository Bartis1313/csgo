#pragma once
#include "config.hpp"

// type - template
// name - name of var
// defaVal - default value for the var
// nameSave - name to be stored in json
#define CONFIG_ADD_VARIABLE(type, name, defVal, nameSave) \
	const size_t name = config.addVar<type>(defVal, XOR(nameSave));
// type - template
// name - name of var
// amount - size of the vector to be filled
// defaVal - default value for the var
// nameSave - name to be stored in json
#define CONFIG_ADD_VEC(type, name, amount, defVal, nameSave) \
	const size_t name = config.addVar<std::vector<type>>(utilities::getFilledVec<type, amount>(defVal), XOR(nameSave));

// need to place them here correctly
struct Variables
{
	CONFIG_ADD_VARIABLE(bool, bBunnyHop, false, "bunnyhop enabled");
	CONFIG_ADD_VARIABLE(int, iChams, 0, "chams mode");
	CONFIG_ADD_VARIABLE(int, iEsp, 0, "esp mode");
	CONFIG_ADD_VARIABLE(bool, bGlow, false, "glow enabled");
	CONFIG_ADD_VARIABLE(bool, bSoundEsp, false, "sound esp enabled");
	CONFIG_ADD_VARIABLE(int, iFOV, 0, "fov for +/- view");
	CONFIG_ADD_VARIABLE(bool, bThirdp, false, "third person enabled");
	CONFIG_ADD_VARIABLE(int, iCrosshair, 0, "crosshair mode");

	// AIMBOT & TRIGGERBOT

	CONFIG_ADD_VARIABLE(bool, bAimbot, false, "aimbot enabled");
	CONFIG_ADD_VARIABLE(int, iAimbot, 0, "aimbot mode");
	CONFIG_ADD_VARIABLE(float, fFovAimbot, 0.0f, "aimbot FOV");
	CONFIG_ADD_VARIABLE(float, fSmooth, 0.0f, "smooth amount for aimbot");
	CONFIG_ADD_VARIABLE(bool, bRCS, false, "recoil control system enabled");
	CONFIG_ADD_VARIABLE(float, fRCS, 50.0f, "recoil control system percentage");
	CONFIG_ADD_VARIABLE(bool, bTriggerbot, false, "triggerbot enabled");
	CONFIG_ADD_VARIABLE(float, fTriggerDelay, 0.0f, "triggerbot amount of delay in ms");


	CONFIG_ADD_VARIABLE(bool, bBacktrack, false, "backtrack enabled");
	CONFIG_ADD_VARIABLE(int, iBacktrackTick, 200, "backtrack amount of ticks to manipulate");
	CONFIG_ADD_VARIABLE(bool, bMenuOpenedx88, true, "x88 menu enabled");
	CONFIG_ADD_VARIABLE(bool, bRadar, false, "2D radar enabled");
	CONFIG_ADD_VARIABLE(bool, bShowInfo, false, "show info enabled");
	CONFIG_ADD_VARIABLE(bool, bShowFlags, false, "show flags enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawInfos, false, "draw infos enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawSkeleton, false, "draw skeleton enabled");
	CONFIG_ADD_VARIABLE(int, iBacktrackChams, 0, "backtrack chams type");
	CONFIG_ADD_VARIABLE(bool, bDLight, false, "DLight enabled");
	CONFIG_ADD_VARIABLE(bool, bEspLasers, false, "Esp Lasers enabled");
	CONFIG_ADD_VARIABLE(bool, bShowPlots, false, "Show plots enabled");
	CONFIG_ADD_VARIABLE(int, iHandChams, 0, "hand chams type");
	CONFIG_ADD_VARIABLE(int, iWeaponChams, 0, "weapon chams type");
	CONFIG_ADD_VARIABLE(bool, bRunNight, false, "run night enabled");
} inline vars;