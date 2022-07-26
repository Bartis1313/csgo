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
	CONFIG_ADD_VARIABLE(CfgColor, cDrawFov, Colors::LightBlue, "draw fov representing aimbot range color");
	CONFIG_ADD_VARIABLE(bool, bDrawBestPoint, false, "draw best point of hitbox");
	CONFIG_ADD_VARIABLE(bool, bAimbotDelay, false, "enable aimbot delay");
	CONFIG_ADD_VARIABLE(float, fAimbotDelay, false, "aimbot delay time");
	CONFIG_ADD_VARIABLE(bool, bAimbotUseKey, false, "Use aimbot on key");
	CONFIG_ADD_VARIABLE(Key, kAimbotKey, Key(KeyMode::DOWN, VK_LBUTTON), "aimbot key");

	// VISUALS

	CONFIG_ADD_VARIABLE(bool, bChams, false, "chams enabled");
	CONFIG_ADD_VARIABLE(bool, bChamsPlayers, false, "chams enabled player");
	CONFIG_ADD_VARIABLE(int, iChamsPlayers, 0, "chams mode Players");
	CONFIG_ADD_VARIABLE(CfgColor, cChamsPlayers, Color(255, 0, 255, 255), "chams color player");
	CONFIG_ADD_VARIABLE(bool, bChamsXQZPlayers, false, "chams XQZ enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cChamsXQZPlayers, Color(0, 100, 255, 255), "chamsXQZ color player");
	CONFIG_ADD_VARIABLE(bool, bChamsWeapons, false, "chams weapons enabled");
	CONFIG_ADD_VARIABLE(bool, bChamsWeaponsDisable, false, "chams weapons enabled to insvisible");
	CONFIG_ADD_VARIABLE(int, iChamsWeapons, 0, "chams mode Weapons");
	CONFIG_ADD_VARIABLE(CfgColor, cChamsWeapons, Color(255, 0, 255, 255), "chams color weapon");
	CONFIG_ADD_VARIABLE(bool, bChamsArmsDisable, false, "chams arms enabled to insvisible");
	CONFIG_ADD_VARIABLE(bool, bChamsArms, false, "chams arms enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cChamsArms, Color(255, 0, 255, 255), "chams color arms");
	CONFIG_ADD_VARIABLE(int, iChamsArms, 0, "chams mode Weapons");
	CONFIG_ADD_VARIABLE(int, iChamsBacktrack, 0, "chams mode backtrack (not style)");
	CONFIG_ADD_VARIABLE(bool, bChamsBacktrack, false, "chams arms enabled backtrack");
	CONFIG_ADD_VARIABLE(CfgColor, cChamsBacktrack, Color(255, 0, 255, 255), "chams color backtrack");
	CONFIG_ADD_VARIABLE(int, iChamsBacktrackMode, 0, "chams mode backtrack");
	CONFIG_ADD_VARIABLE(bool, bChamsbacktrackRainbow, false, "backtrack chams rainbow enabled");
	CONFIG_ADD_VARIABLE(float, fChamsBacktrackRainbowSpeed, 0.5f, "backtrack chams rainbow speed");
	CONFIG_ADD_VARIABLE(bool, bGlow, false, "glow enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cGlow, Colors::Purple, "glow color");
	CONFIG_ADD_VARIABLE(bool, bEsp, false, "ESP enabled");
	CONFIG_ADD_VARIABLE(int, iEsp, 0, "esp mode");
	CONFIG_ADD_VARIABLE(CfgColor, cBox, Colors::Purple, "BOX color");
	CONFIG_ADD_VARIABLE(CfgColor, cBoxFill, Color(0, 0, 0, 140), "BOX fill color");
	CONFIG_ADD_VARIABLE(bool, bBoxMultiColor, false, "BOX multicolor FILL only");
	CONFIG_ADD_VARIABLE(bool, bBoxOutlined, false, "BOX outlined lines");
	CONFIG_ADD_VARIABLE(float, fBoxMultiColor, 1.0f, "BOX multicolor FILL color speed");
	CONFIG_ADD_VARIABLE(bool, bDrawHealth, false, "draw health enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawArmor, false, "draw armor enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawWeapon, false, "draw weapon enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawWeaponTranslate, false, "draw weapon in your lang translated by the game");
	CONFIG_ADD_VARIABLE(CfgColor, cWeaponText, Colors::White, "weapon text color");
	CONFIG_ADD_VARIABLE(CfgColor, cReloadbar, Colors::Turquoise, "weapon reload time bar color");
	CONFIG_ADD_VARIABLE(bool, bDrawName, false, "draw name enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawSkeleton, false, "draw skeleton enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cSkeleton, Colors::White, "skeleton color");
	CONFIG_ADD_VARIABLE(bool, bDLight, false, "DLight enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cDlight, Color(20, 70, 150, 255), "Dlight color");
	CONFIG_ADD_VARIABLE(float, fDlightRadius, 50.0f, "DLight radius");
	CONFIG_ADD_VARIABLE(float, fDlightDecay, 30.0f, "DLight radius");
	CONFIG_ADD_VARIABLE(float, fDlightExponent, 8.0f, "DLight exponent");
	CONFIG_ADD_VARIABLE(bool, bShowInfo, false, "show info enabled");
	CONFIG_ADD_VARIABLE(bool, bShowFlags, false, "show flags enabled");
	CONFIG_ADD_VARIABLE(bool, bEspLasers, false, "Esp Lasers enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawDeadOnly, false, "draw esp only when dead");
	CONFIG_ADD_VEC(bool, vFlags, E2T(EspFlags::FLAGS_SIZE), false, "Esp flags");
	CONFIG_ADD_VARIABLE(bool, bDrawDropped, false, "draw dropped weapons");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawDropped, Colors::White, "draw dropped weapons color");
	CONFIG_ADD_VEC(bool, vDroppedFlags, E2T(DroppedFlags::FLAGS_SIZE), false, "Esp dropped flags");
	CONFIG_ADD_VARIABLE(bool, bSoundEsp, false, "sound esp enabled");
	CONFIG_ADD_VARIABLE(Color, cSoundEsp, Colors::LightBlue, "sound esp color");
	CONFIG_ADD_VARIABLE(float, fStepTime, 10.0f, "max step time");
	CONFIG_ADD_VARIABLE(float, fStepMaxDist, 50.0f, "max distance of steps");
	CONFIG_ADD_VARIABLE(float, fStepMaxLineDist, 80.0f, "max distance of line draw with info");
	CONFIG_ADD_VARIABLE(Color, cStepLine, Colors::White, "step line info color");
	CONFIG_ADD_VARIABLE(bool, bLogEnabled, false, "enable log printing");
	CONFIG_ADD_VARIABLE(float, fLogMaxTime, 4.0f, "time for how long the log should be visible");


	// WORLD

	CONFIG_ADD_VARIABLE(bool, bDrawBomb, false, "draw c4");
	CONFIG_ADD_VARIABLE(CfgColor, cBombBackground, Colors::Black, "bomb background color");
	CONFIG_ADD_VARIABLE(bool, bDrawProjectiles, false, "draw projectiles enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cFlashBang, Color(130, 0, 200, 255), "flashbang color");
	CONFIG_ADD_VARIABLE(CfgColor, cGranede, Color(130, 180, 20, 255), "granede color");
	CONFIG_ADD_VARIABLE(CfgColor, cMolotov, Color(130, 200, 120, 255), "molotov color");
	CONFIG_ADD_VARIABLE(CfgColor, cIncediary, Color(130, 200, 120, 255), "incediary color");
	CONFIG_ADD_VARIABLE(CfgColor, cSmoke, Color(20, 70, 30, 255), "smoke color");
	CONFIG_ADD_VARIABLE(CfgColor, cDecoy, Color(0, 30, 60, 255), "decoy color");
	CONFIG_ADD_VARIABLE(CfgColor, cDropped, Colors::White, "dropped weapons color");
	CONFIG_ADD_VARIABLE(int, iSkyBox, 0, "custom skybox index");
	CONFIG_ADD_VARIABLE(int, iCustomSkyBox, 0, "custom skybox index (not from game)");
	CONFIG_ADD_VARIABLE(bool, bRemoveSky, false, "removes the sky");
	CONFIG_ADD_VARIABLE(bool, bFixProps, false, "fix props in world");
	CONFIG_ADD_VARIABLE(bool, bModulateColor, false, "world edit enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cWorldTexture, Colors::White, "world textture color");
	CONFIG_ADD_VARIABLE(CfgColor, cWorldProp, Colors::White, "world prop color");
	CONFIG_ADD_VARIABLE(CfgColor, cSkyBox, Colors::White, "world skybox color");
	CONFIG_ADD_VARIABLE(float, fShaderParam, 100.0f, "shader percent param");
	CONFIG_ADD_VARIABLE(bool, bDrawmolotovRange, false, "draw mololotov range enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cMolotovRange, Colors::Red.getColorEditAlpha(0.3f), "mololotov range color");
	CONFIG_ADD_VARIABLE(CfgColor, cMolotovRangeText, Colors::White, "mololotov range color for text");
	CONFIG_ADD_VARIABLE(bool, bDrawZeusRange, false, "draw zeus range enabled");
	CONFIG_ADD_VARIABLE(bool, bZeusPartyMode, false, "zeus party mode enabled");
	CONFIG_ADD_VARIABLE(bool, bZeusUseTracing, false, "draw zeus with tracing the circle");
	CONFIG_ADD_VARIABLE(CfgColor, cZeusRange, CfgColor(Colors::Palevioletred, true, 4.0f), "zeus range color");
	CONFIG_ADD_VARIABLE(bool, bDrawSmoke, false, "draw smoke enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawSmoke, Colors::White, "draw smoke color");
	CONFIG_ADD_VARIABLE(bool, bEditEffectsBlood, false, "enabled editing game's effects (BLOOD)");
	CONFIG_ADD_VARIABLE(bool, bEditEffectsMoly, false, "enabled editing game's effects (MOLOTOV)");
	CONFIG_ADD_VARIABLE(bool, bEditEffectsSmoke, false, "enabled editing game's effects (SMOKE)");
	CONFIG_ADD_VARIABLE(CfgColor, cEditMolotov, Colors::Purple, "color molotov edited");
	CONFIG_ADD_VARIABLE(CfgColor, cEditBlood, Colors::Purple, "color blood edited");
	CONFIG_ADD_VARIABLE(CfgColor, cEditSmoke, Colors::Purple, "color smoke edited");
	CONFIG_ADD_VARIABLE(bool, bDrawBulletTracer, false, "bullet tracer enabled");
	CONFIG_ADD_VARIABLE(float, fDrawBulletTracer, 2.0f, "bullet tracer time");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawBulletTracer, Colors::LightBlue, "bullet tracer color");
	CONFIG_ADD_VARIABLE(bool, bDrawClientSideImpacts, false, "draw impacts from client side");
	CONFIG_ADD_VARIABLE(float, fDrawClientSideImpacts, 3.0f, "draw impacts from client side time");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawClientSideImpactsLine, Colors::Cyan, "draw impacts from client side color lines");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawClientSideImpactsFill, Colors::Red, "draw impacts from client side color fill");
	CONFIG_ADD_VARIABLE(bool, bDrawLocalSideImpacts, false, "draw impacts from local side");
	CONFIG_ADD_VARIABLE(float, fDrawLocalSideImpacts, 3.0f, "draw impacts from local side time");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawLocalSideImpactsLine, Colors::Cyan, "draw impacts from local side color lines");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawLocalSideImpactsFill, Colors::Red, "draw impacts from local side color fill");
	CONFIG_ADD_VARIABLE(bool, bFog, false, "fog enabled");
	CONFIG_ADD_VARIABLE(float, fFogDistance, 500.0f, "fog distance");
	CONFIG_ADD_VARIABLE(CfgColor, cFog, Colors::Purple, "fog color");
	CONFIG_ADD_VARIABLE(int, iScreenEffect, 0, "screen effect index");
	CONFIG_ADD_VARIABLE(float, fScreenEffectParam, 1.0f, "screen effect value");
	CONFIG_ADD_VARIABLE(CfgColor, cScreenEffect, Colors::LightBlue, "screen color value");
	CONFIG_ADD_VARIABLE(bool, bControlTone, false, "control tone enabled");
	CONFIG_ADD_VARIABLE(float, fControlToneMin, 0.5f, "control tone min");
	CONFIG_ADD_VARIABLE(float, fControlToneMax, 0.5f, "control tone max");
	CONFIG_ADD_VARIABLE(bool, bWeather, false, "custom weather enabled");
	CONFIG_ADD_VARIABLE(float, fWeatherRainLenght, 0.0f, "weather rain lenght");
	CONFIG_ADD_VARIABLE(float, fWeatherRainSpeed, 0.0f, "weather rain speed");
	CONFIG_ADD_VARIABLE(float, fWeatherRainRadius, 0.0f, "weather rain radius");
	CONFIG_ADD_VARIABLE(float, fWeatherRainWidth, 0.0f, "weather rain width");
	CONFIG_ADD_VARIABLE(float, fWeatherRainSideVel, 0.0f, "weather rain side vel");
	CONFIG_ADD_VARIABLE(float, fWeatherRainAlpha, 0.0f, "weather rain alpha");
	CONFIG_ADD_VARIABLE(float, fWeatherWindSpeed, 0.0f, "weather rain wind speed");

	// MASTER SWITCHES

	CONFIG_ADD_VARIABLE(bool, bMenuOpenedx88, true, "x88 menu enabled");
	CONFIG_ADD_VARIABLE(Key, kMenu, Key(KeyMode::TOGGLE, VK_INSERT), "key for imgui menu");
	CONFIG_ADD_VARIABLE(Key, kPanic, Key(KeyMode::DOWN, VK_DELETE), "key for shutdown");
	CONFIG_ADD_VARIABLE(Key, kConsoleLog, Key(KeyMode::TOGGLE, VK_HOME), "key for log console");


	// MISC

	CONFIG_ADD_VARIABLE(float, fFOV, 0, "fov for +/- view");
	CONFIG_ADD_VARIABLE(bool, bThirdp, false, "third person enabled");
	CONFIG_ADD_VARIABLE(Key, kThirdp, Key(KeyMode::TOGGLE, 0x56), "third person key");
	CONFIG_ADD_VARIABLE(float, fThirdpDistance, 200.0f, "third person distance");
	CONFIG_ADD_VARIABLE(float, fThirdpX, 0.0f, "third personX");
	CONFIG_ADD_VARIABLE(float, fThirdpY, 0.0f, "third personY");
	CONFIG_ADD_VARIABLE(int, iCrosshair, 0, "crosshair mode");
	CONFIG_ADD_VARIABLE(bool, bBacktrack, false, "backtrack enabled");
	CONFIG_ADD_VARIABLE(float, fBacktrackTick, 200.0f, "backtrack amount of ticks to manipulate");
	CONFIG_ADD_VARIABLE(bool, bFakeLatency, false, "fake latency enabled");
	CONFIG_ADD_VARIABLE(float, fFakeLatency, 200.0f, "fake latency amount");
	CONFIG_ADD_VARIABLE(bool, bBunnyHop, false, "bunnyhop enabled");
	CONFIG_ADD_VARIABLE(int, iBunnyHopChance, 0, "bunnyhop chance");
	CONFIG_ADD_VARIABLE(int, iAutoStrafe, 0, "autostrafe mode");
	CONFIG_ADD_VARIABLE(bool, bDrawHitmarker, false, "draw hitmarker enabled");
	CONFIG_ADD_VARIABLE(bool, bPlayHitmarker, false, "play sound hitmarker enabled");
	CONFIG_ADD_VARIABLE(bool, bDrawHitmarker3D, false, "draw hitmarker enabled in 3D");
	CONFIG_ADD_VARIABLE(bool, bDrawHitmarkerResize, false, "draw hitmarker with resizing lines");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawHitmarkerNormal, Colors::White, "draw hitmarker normal hit");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawHitmarkerHead, Colors::Pink, "draw hitmarker head hit");
	CONFIG_ADD_VARIABLE(CfgColor, cDrawHitmarkerDead, Colors::Green, "draw hitmarker dead hit");
	CONFIG_ADD_VARIABLE(float, fHitmarkerTime, 0.8f, "hitmarker time");
	CONFIG_ADD_VARIABLE(bool, bNoScope, false, "no scope enabled");
	CONFIG_ADD_VARIABLE(bool, bShowFpsPlot, false, "show fps plot");
	CONFIG_ADD_VARIABLE(bool, bShowVelocityPlot, false, "show velocity plot");
	CONFIG_ADD_VARIABLE(CfgColor, cVelocityPlot, Colors::White, "lines velocity plot color");
	CONFIG_ADD_VARIABLE(bool, bDrawMiscInfo, false, "draw misc info");
	CONFIG_ADD_VARIABLE(bool, bAimingWarn, false, "aiming at you warning enabled");
	CONFIG_ADD_VARIABLE(bool, bRadar, false, "2D radar enabled");
	CONFIG_ADD_VARIABLE(CfgColor, cRadarLine, Colors::White, "radar player lines color");
	CONFIG_ADD_VARIABLE(CfgColor, cRadarPlayer, Colors::Purple, "radar player color");
	CONFIG_ADD_VARIABLE(float, fRadarThickness, 5.0f, "radar thickness");
	CONFIG_ADD_VARIABLE(float, fRadarLenght, 20.0f, "radar lenght of line");
	CONFIG_ADD_VARIABLE(float, fRadarScale, 1.8f, "radar scaling");
	CONFIG_ADD_VARIABLE(bool, bRadarRanges, true, "enable radar ranges");
	CONFIG_ADD_VARIABLE(float, fRadarSize, 200.0f, "Radar size");
	CONFIG_ADD_VARIABLE(int, iRunMovementTrail, 0, "movement trail line type");
	CONFIG_ADD_VARIABLE(bool, bRunMovementTrail, false, "enable movement trail line");
	CONFIG_ADD_VARIABLE(CfgColor, cMovementTrail, Colors::Coral, "movement trail color");
	CONFIG_ADD_VARIABLE(bool, bMovementRainbow, false, "prefer rainbow color for trail");
	CONFIG_ADD_VARIABLE(float, fMovementRainbowSpeed, 3.0f, "speed of color change (movement trail)");
	CONFIG_ADD_VARIABLE(float, fMovementBeamSpeed, 2.0f, "speed of beam movement");
	CONFIG_ADD_VARIABLE(float, fMovementLife, 4.0f, "life of beam movement");
	CONFIG_ADD_VARIABLE(bool, bNadePred, false, "nade prediction trace");
	CONFIG_ADD_VARIABLE(bool, bNadePredAlways, false, "nade prediction will show line even if not throwing nade");
	CONFIG_ADD_VARIABLE(Color, cNadePredColor, Colors::LightBlue, "nade prediction line color");
	CONFIG_ADD_VARIABLE(Color, cNadeBoxColorFill, Colors::Green, "nade prediction cube fill color");
	CONFIG_ADD_VARIABLE(Color, cNadeBoxColorOutline, Colors::Green, "nade prediction cube outline color");
	CONFIG_ADD_VARIABLE(bool, bHat, false, "draw a cone on local player's head");
	CONFIG_ADD_VARIABLE(bool, bHatRainbow, false, "is hat a rainbow");
	CONFIG_ADD_VARIABLE(float, fHatSpeed, 3.0f, "rainbow hat speed");
	CONFIG_ADD_VARIABLE(float, fHatSize, -30.0f, "hat size");
	CONFIG_ADD_VARIABLE(float, fHatRadius, 20.0f, "hat radius");
	CONFIG_ADD_VARIABLE(int, iHatTriangleAlpha, 70, "hat rainbow triangle alpha");
	CONFIG_ADD_VARIABLE(int, iHatLinesAlpha, 180, "hat rainbow lines alpha");
	CONFIG_ADD_VARIABLE(CfgColor, cHatTriangle, Colors::Palevioletred, "color for the hat, triangle");
	CONFIG_ADD_VARIABLE(CfgColor, cHatLine, Colors::Palevioletred, "color for the hat, line");
	CONFIG_ADD_VARIABLE(CfgColor, cFps, Colors::White, "color fps line");
	CONFIG_ADD_VARIABLE(float, fFPSCap, false, "FPS value for plot (in game)");
	CONFIG_ADD_VARIABLE(bool, bFPSCustom, false, "helper window for fps");
	CONFIG_ADD_VARIABLE(bool, bVelocityCustom, false, "helper window for velocity");
	CONFIG_ADD_VARIABLE(bool, bRemoveBloodSpray, false, "[DEPRECATED] remove blood spray");
	CONFIG_ADD_VARIABLE(bool, bNadeTracer, false, "run nade tracer");
	CONFIG_ADD_VARIABLE(Color, cNadeTracer, Colors::LightBlue, "nade tracer color");
	CONFIG_ADD_VARIABLE(float, fNadeTracerMaxDist, 30.0f, "max meters for nade tracer");
	CONFIG_ADD_VARIABLE(bool, bNadeTracerWarn, false, "run nade tracer warning");
	CONFIG_ADD_VARIABLE(bool, bDiscord, false, "run discord rpc");
	CONFIG_ADD_VARIABLE(bool, bFreeLook, false, "freelook enable");
	CONFIG_ADD_VARIABLE(Key, kFreeLook, Key(KeyMode::DOWN, VK_MENU), "freelook key");
	CONFIG_ADD_VARIABLE(bool, bMirrorCam, false, "mirrorcam enable");
	CONFIG_ADD_VARIABLE(bool, bMirrorCamOnKey, false, "mirrorcam enable only on key");
	CONFIG_ADD_VARIABLE(Key, kMirrorCam, Key(KeyMode::TOGGLE, VK_F1), "mirrorcam key");
	CONFIG_ADD_VARIABLE(bool, bFreeCam, false, "freecam enable");
	CONFIG_ADD_VARIABLE(float, fFreeCam, 1.0f, "freecam speed");
	CONFIG_ADD_VARIABLE(Key, kFreeCam, Key(KeyMode::TOGGLE, VK_F2), "freecam key");
	CONFIG_ADD_VARIABLE(bool, bFlashlight, false, "flashlight enable");
	CONFIG_ADD_VARIABLE(Key, kFlashlight, Key(KeyMode::TOGGLE, 0x4C), "flashlight key"); // L key
	CONFIG_ADD_VARIABLE(bool, bFlashlightBigMode, false, "flashlight big mode enabled");
	CONFIG_ADD_VARIABLE(float, fFlashlightFov, 30.0f, "flashlight fov");

	// EFFECTS - TODO sort this

	CONFIG_ADD_VARIABLE(bool, bMotionBlur, false, "motion blur enabled");
	CONFIG_ADD_VARIABLE(bool, bMotionBlurForward, false, "motion blur enabled forward");
	CONFIG_ADD_VARIABLE(float, fMotionBlurFallingIntensity, 1.0f, "motion blur FallingIntensity");
	CONFIG_ADD_VARIABLE(float, fMotionBlurFallingMin, 10.0f, "motion blur FallingMin");
	CONFIG_ADD_VARIABLE(float, fMotionBlurFallingMax, 20.0f, "motion blur FallingMax");
	CONFIG_ADD_VARIABLE(float, fMotionBlurGlobalStrength, 1.0f, "motion blur GlobalStrength");
	CONFIG_ADD_VARIABLE(float, fMotionBlurRotationIntensity, 0.15f, "motion blur RotationIntensity");
	CONFIG_ADD_VARIABLE(float, fMotionBlurRollIntensity, 0.3f, "motion blur RollIntensity");

	// STYLING

	CONFIG_ADD_VARIABLE(int, iStyleMenu, 0, "style of the menu");
	CONFIG_ADD_VARIABLE(bool, bBackround, false, "run menu background");
	CONFIG_ADD_VARIABLE(float, fBackground, 2.0f, "background speed");
	CONFIG_ADD_VARIABLE(float, fBackgroundDistance, 200.0f, "background lines max distance");
	CONFIG_ADD_VARIABLE(int, iBackgroundSize, 150, "background amount of records");
	CONFIG_ADD_VARIABLE(CfgColor, cBackGround1, Color(220, 10, 100, 255), "background color1");
	CONFIG_ADD_VARIABLE(CfgColor, cBackGround2, Color(65, 75, 105, 255), "background color2");
	CONFIG_ADD_VARIABLE(CfgColor, cBackGround3, Color(220, 240, 240, 255), "background color3");

} inline vars;
#pragma endregion
