#pragma once

#include <Windows.h>

#include "config.hpp"
#include "enums.hpp"

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
	CONFIG_ADD_VARIABLE(float, fRCSx, 50.0f, "recoil control system percentage for axis");
	CONFIG_ADD_VARIABLE(float, fRCSy, 50.0f, "recoil control system percentage for yaw");
	CONFIG_ADD_VARIABLE(bool, bTriggerbot, false, "triggerbot enabled");
	CONFIG_ADD_VARIABLE(float, fTriggerDelay, 0.0f, "triggerbot amount of delay in ms");
	CONFIG_ADD_VARIABLE(bool, bDrawFov, false, "draw fov representing aimbot range");
	CONFIG_ADD_VARIABLE(Color, cDrawFov, Colors::LightBlue, "draw fov representing aimbot range color");
	CONFIG_ADD_VARIABLE(bool, bDrawBestPoint, false, "draw best point of hitbox");
	CONFIG_ADD_VARIABLE(bool, bAimbotDelay, false, "enable aimbot delay");
	CONFIG_ADD_VARIABLE(float, fAimbotDelay, false, "aimbot delay time");

	// VISUALS
	CONFIG_ADD_VARIABLE(bool, bChams, false, "chams enabled");
	CONFIG_ADD_VARIABLE(bool, bChamsPlayers, false, "chams enabled player");
	CONFIG_ADD_VARIABLE(int, iChamsPlayers, 0, "chams mode Players");
	CONFIG_ADD_VARIABLE(Color, cChamsPlayers, Color(255, 0, 255, 255), "chams color player");
	CONFIG_ADD_VARIABLE(bool, bChamsXQZPlayers, false, "chams XQZ enabled");
	CONFIG_ADD_VARIABLE(Color, cChamsXQZPlayers, Color(0, 100, 255, 255), "chamsXQZ color player");
	CONFIG_ADD_VARIABLE(bool, bChamsWeapons, false, "chams weapons enabled");
	CONFIG_ADD_VARIABLE(bool, bChamsWeaponsDisable, false, "chams weapons enabled to insvisible");
	CONFIG_ADD_VARIABLE(int, iChamsWeapons, 0, "chams mode Weapons");
	CONFIG_ADD_VARIABLE(Color, cChamsWeapons, Color(255, 0, 255, 255), "chams color weapon");
	CONFIG_ADD_VARIABLE(bool, bChamsArmsDisable, false, "chams arms enabled to insvisible");
	CONFIG_ADD_VARIABLE(bool, bChamsArms, false, "chams arms enabled");
	CONFIG_ADD_VARIABLE(Color, cChamsArms, Color(255, 0, 255, 255), "chams color arms");
	CONFIG_ADD_VARIABLE(int, iChamsArms, 0, "chams mode Weapons");
	CONFIG_ADD_VARIABLE(int, iChamsBacktrack, 0, "chams mode backtrack (not style)");
	CONFIG_ADD_VARIABLE(bool, bChamsBacktrack, false, "chams arms enabled backtrack");
	CONFIG_ADD_VARIABLE(Color, cChamsBacktrack, Color(255, 0, 255, 255), "chams color backtrack");
	CONFIG_ADD_VARIABLE(int, iChamsBacktrackMode, 0, "chams mode backtrack");
	CONFIG_ADD_VARIABLE(bool, bChamsbacktrackRainbow, false, "backtrack chams rainbow enabled");
	CONFIG_ADD_VARIABLE(float, fChamsBacktrackRainbowSpeed, 0.5f, "backtrack chams rainbow speed");
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
	CONFIG_ADD_VARIABLE(bool, bDrawName, false, "draw name enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawSkeleton, false, "draw skeleton enabled");
	CONFIG_ADD_VARIABLE(Color, cSkeleton, Colors::White, "skeleton color");
	CONFIG_ADD_VARIABLE(Color, cDlight, Color(20, 70, 150, 255), "Dlight color");
	CONFIG_ADD_VARIABLE(bool, bShowInfo, false, "show info enabled");
	CONFIG_ADD_VARIABLE(bool, bShowFlags, false, "show flags enabled");
	CONFIG_ADD_VARIABLE(bool, bDLight, false, "DLight enabled");
	CONFIG_ADD_VARIABLE(bool, bEspLasers, false, "Esp Lasers enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawDeadOnly, false, "draw esp only when dead");
	CONFIG_ADD_VEC(bool, vFlags, E2T(EspFlags::FLAGS_SIZE), false, "Esp flags");
	CONFIG_ADD_VARIABLE(bool, bDrawDropped, false, "draw dropped weapons");
	CONFIG_ADD_VARIABLE(Color, cDrawDropped, Colors::White, "draw dropped weapons color");
	CONFIG_ADD_VEC(bool, vDroppedFlags, E2T(DroppedFlags::FLAGS_SIZE), false, "Esp flags");


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
	CONFIG_ADD_VARIABLE(int, iSkyBox, 0, "custom skybox index");
	CONFIG_ADD_VARIABLE(bool, bRemoveSky, false, "removes the sky");
	CONFIG_ADD_VARIABLE(bool, bFixProps, false, "fix props in world");
	CONFIG_ADD_VARIABLE(bool, bModulateColor, false, "world edit enabled");
	CONFIG_ADD_VARIABLE(Color, cWorldTexture, Colors::White, "world textture color");
	CONFIG_ADD_VARIABLE(Color, cWorldProp, Colors::White, "world prop color");
	CONFIG_ADD_VARIABLE(Color, cSkyBox, Colors::White, "world skybox color");
	CONFIG_ADD_VARIABLE(float, fShaderParam, 100.0f, "shader percent param");
	CONFIG_ADD_VARIABLE(bool, bDrawmolotovRange, false, "draw mololotov range enabled");
	CONFIG_ADD_VARIABLE(Color, cMolotovRange, Colors::Red, "mololotov range color");
	CONFIG_ADD_VARIABLE(Color, cMolotovRangeText, Colors::White, "mololotov range color for text");
	CONFIG_ADD_VARIABLE(bool, bDrawZeusRange, false, "draw zeus range enabled");
	CONFIG_ADD_VARIABLE(Color, cZeusRange, Colors::Palevioletred, "zeus range color");
	CONFIG_ADD_VARIABLE(bool, bDrawSmoke, false, "draw smoke enabled");
	CONFIG_ADD_VARIABLE(Color, cDrawSmoke, Colors::White, "draw smoke color");
	CONFIG_ADD_VARIABLE(bool, bEditMolotov, false, "enabled editing molotov color");
	CONFIG_ADD_VARIABLE(Color, cEditMolotov, Colors::Purple, "color molotov edited");


	// MASTER SWITCHES
	
	CONFIG_ADD_VARIABLE(bool, bMenuOpenedx88, true, "x88 menu enabled");
	CONFIG_ADD_VARIABLE(int, iKeyMenu, VK_INSERT, "key for imgui menu");
	CONFIG_ADD_VARIABLE(int, iKeyPanic, VK_DELETE, "key for shutdown");
	CONFIG_ADD_VARIABLE(int, iKeyConsoleLog, VK_HOME, "key for log console");
	CONFIG_ADD_VARIABLE(std::string, sLoadName, XOR("default.cfg"), "config to load (you only care for this in default cfg)");
	

	// MISC

	CONFIG_ADD_VARIABLE(bool, bSoundEsp, false, "sound esp enabled");
	CONFIG_ADD_VARIABLE(float, fFOV, 0, "fov for +/- view");
	CONFIG_ADD_VARIABLE(bool, bThirdp, false, "third person enabled");
	CONFIG_ADD_VARIABLE(int, iCrosshair, 0, "crosshair mode");
	CONFIG_ADD_VARIABLE(bool, bBacktrack, false, "backtrack enabled");
	CONFIG_ADD_VARIABLE(float, fBacktrackTick, 200.0f, "backtrack amount of ticks to manipulate");
	CONFIG_ADD_VARIABLE(bool, bBunnyHop, false, "bunnyhop enabled");
	CONFIG_ADD_VARIABLE(bool, bAutoStrafe, false, "autostrafe enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawHitmarker, false, "draw hitmarker enabled");
	CONFIG_ADD_VARIABLE(bool, bPlayHitmarker, false, "play sound hitmarker enabled");
	CONFIG_ADD_VARIABLE(float, fHitmarkerTime, 0.8f, "hitmarker time");
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
	CONFIG_ADD_VARIABLE(int, iRunMovementTrail, 0, "movement trail line type");
	CONFIG_ADD_VARIABLE(bool, bRunMovementTrail, false, "enable movement trail line");
	CONFIG_ADD_VARIABLE(Color, cMovementTrail, Colors::Coral, "movement trail color");
	CONFIG_ADD_VARIABLE(bool, bMovementRainbow, false, "prefer rainbow color for trail");
	CONFIG_ADD_VARIABLE(float, fMovementRainbowSpeed, 3.0f, "speed of color change (movement trail)");
	CONFIG_ADD_VARIABLE(float, fMovementBeamSpeed, 2.0f, "speed of beam movement");
	CONFIG_ADD_VARIABLE(float, fMovementLife, 4.0f, "life of beam movement");
	CONFIG_ADD_VARIABLE(bool, bNadePred, false, "nade prediction trace");
	CONFIG_ADD_VARIABLE(bool, bNadePredAlways, false, "nade prediction will show line even if not throwing nade");
	CONFIG_ADD_VARIABLE(bool, bHat, false, "draw a cone on local player's head");
	CONFIG_ADD_VARIABLE(bool, bHatRainbow, false, "is hat a rainbow");
	CONFIG_ADD_VARIABLE(float, fHatSpeed, 3.0f, "rainbow hat speed");
	CONFIG_ADD_VARIABLE(float, fHatSize, -30.0f, "hat size");
	CONFIG_ADD_VARIABLE(float, fHatRadius, 20.0f, "hat radius");
	CONFIG_ADD_VARIABLE(int, iHatTriangleAlpha, 70, "hat rainbow triangle alpha");
	CONFIG_ADD_VARIABLE(int, iHatLinesAlpha, 180, "hat rainbow lines alpha");
	CONFIG_ADD_VARIABLE(Color, cHatTriangle, Colors::Palevioletred, "color for the hat, triangle");
	CONFIG_ADD_VARIABLE(Color, cHatLine, Colors::Palevioletred, "color for the hat, line");
	CONFIG_ADD_VARIABLE(float, fFPSCap, false, "FPS value for plot (in game)");
	CONFIG_ADD_VARIABLE(bool, bFPSCustom, false, "helper window for fps");
	CONFIG_ADD_VARIABLE(bool, bVelocityCustom, false , "helper window for velocity");

	// STYLING

	CONFIG_ADD_VARIABLE(int, iStyleMenu, 0, "style of the menu");

} inline vars;
#pragma endregion
