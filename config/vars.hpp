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

#pragma region vars
struct Variables
{

	// AIMBOT & TRIGGERBOT

	CONFIG_ADD_VARIABLE(bool, bAimbot, false, "aimbot enabled");
	CONFIG_ADD_VARIABLE(int, iAimbot, 0, "aimbot mode");
	CONFIG_ADD_VARIABLE(float, fFovAimbot, 0.0f, "aimbot FOV");
	CONFIG_ADD_VARIABLE(float, fSmooth, 1.0f, "smooth amount for aimbot");
	CONFIG_ADD_VARIABLE(bool, bRCS, false, "recoil control system enabled");
	CONFIG_ADD_VARIABLE(float, fRCS, 50.0f, "recoil control system percentage");
	CONFIG_ADD_VARIABLE(bool, bTriggerbot, false, "triggerbot enabled");
	CONFIG_ADD_VARIABLE(float, fTriggerDelay, 0.0f, "triggerbot amount of delay in ms");
	CONFIG_ADD_VARIABLE(bool, bDrawFov, false, "draw fov representing aimbot range");
	CONFIG_ADD_VARIABLE(Color, cDrawFov, Colors::LightBlue, "draw fov representing aimbot range color");

	// VISUALS

	CONFIG_ADD_VARIABLE(bool, bChams, false, "chams enabled");
	CONFIG_ADD_VARIABLE(int, iChams, 0, "chams mode");
	CONFIG_ADD_VARIABLE(Color, cChams, Color(255, 0, 255, 255), "chams color");
	CONFIG_ADD_VARIABLE(Color, cChamsXYZ, Color(0, 100, 255, 255), "chamsXYZ color");
	CONFIG_ADD_VARIABLE(bool, bGlow, false, "glow enabled");
	CONFIG_ADD_VARIABLE(Color, cGlow, Colors::Purple, "glow color");
	CONFIG_ADD_VARIABLE(bool, bEsp, false, "ESP enabled");
	CONFIG_ADD_VARIABLE(int, iEsp, 0, "esp mode");
	CONFIG_ADD_VARIABLE(Color, cBox, Colors::Purple, "BOX color");
	CONFIG_ADD_VARIABLE(Color, cBoxFill, Color(0, 0, 0, 140), "BOX fill color");
	CONFIG_ADD_VARIABLE(bool, bDrawHealth, false, "draw health enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawArmor, false, "draw armor enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawWeapon, false, "draw weapon enabled");
	CONFIG_ADD_VARIABLE(Color, cWeaponText, Colors::White, "weapon text color");
	CONFIG_ADD_VARIABLE(Color, cReloadbar, Colors::Turquoise, "weapon reload time bar color");
	CONFIG_ADD_VARIABLE(bool, bDrawInfos, false, "draw infos enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawSkeleton, false, "draw skeleton enabled");
	CONFIG_ADD_VARIABLE(Color, cSkeleton, Colors::White, "skeleton color");
	CONFIG_ADD_VARIABLE(Color, cDlight, Color(20, 70, 150, 255), "Dlight color");
	CONFIG_ADD_VARIABLE(bool, bShowInfo, false, "show info enabled");
	CONFIG_ADD_VARIABLE(bool, bShowFlags, false, "show flags enabled");
	CONFIG_ADD_VARIABLE(bool, bBacktrackChams, false, "enabled backtrack chams");
	CONFIG_ADD_VARIABLE(int, iBacktrackChams, 0, "backtrack chams type");
	CONFIG_ADD_VARIABLE(bool, bDLight, false, "DLight enabled");
	CONFIG_ADD_VARIABLE(bool, bEspLasers, false, "Esp Lasers enabled");
	CONFIG_ADD_VARIABLE(int, iHandChams, 0, "hand chams type");
	CONFIG_ADD_VARIABLE(int, iWeaponChams, 0, "weapon chams type");
	CONFIG_ADD_VARIABLE(Color, cBackTrackChams, Colors::LightBlue, "backtrack chams color");


	// WORLD

	CONFIG_ADD_VARIABLE(bool, bDrawDroppedC4, false, "draw dropped c4 enabled");
	CONFIG_ADD_VARIABLE(Color, cDrawDroppedC4, Colors::White, "draw dropped c4 color");
	CONFIG_ADD_VARIABLE(bool, bDrawBomb, false, "draw c4");
	CONFIG_ADD_VARIABLE(Color, cDrawBomb, Colors::White, "draw c4 color");
	CONFIG_ADD_VARIABLE(bool, bDrawProjectiles, false, "draw projectiles enabled");
	CONFIG_ADD_VARIABLE(Color, cFlashBang, Color(130, 0, 200, 255), "flashbang color");
	CONFIG_ADD_VARIABLE(Color, cGranede, Color(130, 180, 20, 255), "granede color");
	CONFIG_ADD_VARIABLE(Color, cMolotov, Color(130, 200, 120, 255), "molotov color");
	CONFIG_ADD_VARIABLE(Color, cIncediary, Color(130, 200, 120, 255), "incediary color");
	CONFIG_ADD_VARIABLE(Color, cSmoke, Color(20, 70, 30, 255), "smoke color");
	CONFIG_ADD_VARIABLE(Color, cDecoy, Color(0, 30, 60, 255), "decoy color");
	CONFIG_ADD_VARIABLE(Color, cDropped, Colors::White, "dropped weapons color");
	CONFIG_ADD_VARIABLE(bool, bRunNight, false, "run night enabled");
	CONFIG_ADD_VARIABLE(Color, cSky, Colors::White, "sky color");
	CONFIG_ADD_VARIABLE(float, fBrightnessWorld, 40.0f, "Brightness World");
	CONFIG_ADD_VARIABLE(bool, bDrawmolotovRange, false, "draw mololotov range enabled");
	CONFIG_ADD_VARIABLE(Color, cMolotovRange, Colors::Red, "mololotov range color");
	CONFIG_ADD_VARIABLE(Color, cMolotovRangeText, Colors::White, "mololotov range color for text");
	CONFIG_ADD_VARIABLE(bool, bDrawZeusRange, false, "draw zeus range enabled");
	CONFIG_ADD_VARIABLE(Color, cZeusRange, Colors::Palevioletred, "zeus range color");

	// MASTER SWITCHES
	
	CONFIG_ADD_VARIABLE(bool, bMenuOpenedx88, true, "x88 menu enabled");
	CONFIG_ADD_VARIABLE(int, iKeyMenu, VK_INSERT, "key for imgui menu");
	CONFIG_ADD_VARIABLE(int, iKeyPanic, VK_DELETE, "key for shutdown");
	

	// MISC

	CONFIG_ADD_VARIABLE(bool, bSoundEsp, false, "sound esp enabled");
	CONFIG_ADD_VARIABLE(float, fFOV, 0, "fov for +/- view");
	CONFIG_ADD_VARIABLE(bool, bThirdp, false, "third person enabled");
	CONFIG_ADD_VARIABLE(bool, bCrosshair, false, "enable crosshair");
	CONFIG_ADD_VARIABLE(int, iCrosshair, 0, "crosshair mode");
	CONFIG_ADD_VARIABLE(bool, bBacktrack, false, "backtrack enabled");
	CONFIG_ADD_VARIABLE(float, fBacktrackTick, 200.0f, "backtrack amount of ticks to manipulate");
	CONFIG_ADD_VARIABLE(bool, bBunnyHop, false, "bunnyhop enabled");
	CONFIG_ADD_VARIABLE(bool, bAutoStrafe, false, "autostrafe enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawHitmarker, false, "draw hitmarker enabled");
	CONFIG_ADD_VARIABLE(bool, bPlayHitmarker, false, "play sound hitmarker enabled");
	CONFIG_ADD_VARIABLE(bool, bNoScope, false, "no scope enabled");
	CONFIG_ADD_VARIABLE(bool, bShowFpsPlot, false, "show fps plot");
	CONFIG_ADD_VARIABLE(bool, bShowVelocityPlot, false, "show velocity plot");
	CONFIG_ADD_VARIABLE(bool, bDrawMiscInfo, false, "draw misc info");
	CONFIG_ADD_VARIABLE(bool, bAimingWarn, false, "aiming at you warning enabled");
	CONFIG_ADD_VARIABLE(bool, bRadar, false, "2D radar enabled");
	CONFIG_ADD_VARIABLE(Color, cRadarLine, Colors::White, "radar player lines color");
	CONFIG_ADD_VARIABLE(Color, cRadarPlayer, Colors::Purple, "radar player color");
	CONFIG_ADD_VARIABLE(float, fRadarThickness, 5.0f, "radar thickness");
	CONFIG_ADD_VARIABLE(float, fRadarLenght, 20.0f, "radar lenght of line");
	CONFIG_ADD_VARIABLE(float, fRadarScale, 1.8f, "radar scaling");
	CONFIG_ADD_VARIABLE(bool, bRadarRanges, true, "enable radar ranges");
	CONFIG_ADD_VARIABLE(bool, bRunMovementTrail, false, "enable movement trail line");
	CONFIG_ADD_VARIABLE(Color, cMovementTrail, Colors::Coral, "movement trail color");
	CONFIG_ADD_VARIABLE(bool, bMovementRainbow, false, "prefer rainbow color for trail");
	CONFIG_ADD_VARIABLE(float, fMovementRainbowSpeed, 3.0f, "speed of color change (movement trail)");
	CONFIG_ADD_VARIABLE(float, fMovementBeamSpeed, 2.0f, "speed of beam movement");
	CONFIG_ADD_VARIABLE(float, fMovementLife, 4.0f, "life of beam movement");
	CONFIG_ADD_VARIABLE(bool, bNadePred, false, "nade prediction trace");
	CONFIG_ADD_VARIABLE(bool, bNadePredAlways, false, "nade prediction will show line even if not throwing nade");

} inline vars;
#pragma endregion

#pragma region enums
enum class AimbotID : int
{
	NEAREST = 0,
	HEAD,
	CHEST
};

enum class CrossHairTypes : int
{
	OFF,
	STATIC,
	RECOIL,
	ENGINE
};

enum class ChamsID : int
{
	STATIC = 1,
	XYZ,
};

enum class BTChamsID : int
{
	STABLE = 1,
	LAST_TICK,
	RAINBOW,
};

enum class HandTypes : int
{
	COLOR = 0,
	NO_HANDS,
	NO_WEAPON,
};

enum class BoxTypes : int
{
	BOX2D = 0,
	FILLED2D,
	BOX3D,
	FILLED3D
};
#pragma endregion