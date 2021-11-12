#pragma once
#include "Item.hpp"
#include <map>

namespace vars
{

	inline bool bBunnyHop = false;

	inline int iChams = 0;

	inline int iEsp = 0;

	inline bool bGlow = false;

	inline bool bSoundEsp = false;

	inline int iFOV = 0;

	inline bool bThirdp = false;
	
	inline int iCrosshair = 0;

	inline int iAimbot = 0;
	inline int iFovAimbot = 5;
	inline int iSmooth = 0;
	inline int iRCS = 50;

	inline bool bTriggerbot = false;
	inline int iTriggerDelay = 0;

	inline bool bBacktrack = false;
	inline int iBacktrackTick = 200;

	inline bool bMenuOpen = true;

	inline bool bRadar = false;

	inline bool bShowInfo = false;

	inline bool bShowFlags = false;
	inline bool bDrawInfos = false;

	inline bool bDrawSkeleton = false;

	inline int iBacktrackChams = 0;
	
	inline Item options[] =
	{
		Item{ "BunnyHop", &bBunnyHop }, // 0
		Item{ "Chams", &iChams, {"OFF", "Static", "Xyz"} }, // 1
		Item{ "Esp", &iEsp, {"OFF", "2D", "2D FILLED", "3D", "3D FILLED"} }, // 2
		Item{ "Glow", &bGlow }, // 3
		Item{ "Sound Esp", &bSoundEsp }, // 4
		Item{ "Fov", &iFOV, {-35, 35} }, // 5
		Item{ "ThirdP: ", &bThirdp }, // 6
		Item{ "Aimbot", &iAimbot,  {"OFF", "Nearest", "Head", "Chest"} }, // 7
		Item{ "AimFov", &iFovAimbot, {0, 15} }, // 8
		Item{ "Smooth", &iSmooth, {0, 50} }, // 9
		Item{ "Rcs", &iRCS, {0, 100} }, // 10
		Item{ "Xhair", &iCrosshair, {"OFF", "Static", "Recoil", "Engine"} }, // 11
		Item{ "Triggerbot", &bTriggerbot }, // 12
		Item{ "Trigger Delay", &iTriggerDelay, {0, 1000} }, // 13
		Item{ "Backtrack", &bBacktrack}, // 14
		Item{ "Backtrack ms", &iBacktrackTick, {1, 200} }, // 15
		Item{ "Radar 2D", &bRadar }, // 16
		Item{ "Local Info", &bShowInfo }, // 17
		Item{ "Draw Flags", &bShowFlags }, // 18
		Item{ "Draw Infos", &bDrawInfos }, // 19
		Item{ "Skeleton", &bDrawSkeleton }, // 20
		Item{ "BT Chams", &iBacktrackChams, {"OFF", "Stable", "Last tick", "Rainbow"} }, // 21
	};
	
	inline std::map<std::string, int> names =
	{
		{"bunnyhop", 0},
		{"chams", 1},
		{"esp", 2},
		{"glow", 3},
		{"soundesp", 4},
		{"fov", 5},
		{"thirdp", 6},
		{"aimbot", 7},
		{"aimbot_fov", 8},
		{"smooth", 9},
		{"rcs", 10},
		{"xhair", 11},
		{"triggerbot", 12},
		{"triggerbot_delay", 13},
		{"backtrack", 14},
		{"backtrack_ms", 15},
		{"radar", 16},
		{"local_info", 17},
		{"esp_flags", 18},
		{"esp_info", 19},
		{"esp_skeleton", 20},
		{"bt_chams", 21}
	};
};
