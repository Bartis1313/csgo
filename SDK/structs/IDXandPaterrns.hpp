#pragma once

#include "../../utilities/utilities.hpp"

enum VTableIndexes
{
	ISPLAYER = 158,
	ISWEAPON = 166,
	INACCURACY = 483,
	SPREAD = 453,
	WEAPONINFO = 461,
	PUNCH = 346,
	ABS_ORIGIN = 10,
	ABS_ANGLE = 11,
	CLIENT_CLASS = 2,
	COLLIDEABLE = 3,
	GET_INDEX = 10,
	RENDER_BOUNDS = 17,
	SETUP_BONES = 13,
	GET_MODEL = 8,
	DRAW_MODEL = 9,
	IS_DORMANT = 9,
	RENDERABLE_TO_WORLD = 32,
	RELEASE = 1,
	PRE_DATA_CHANGED = 4,
	DATA_CHANGED = 5,
	PRE_DATA_UPDATE = 6,
	POST_DATA_UPDATE = 7,
};

#define ENGINE_DLL					XOR("engine.dll")
#define CLIENT_DLL					XOR("client.dll")
#define VSTD_DLL					XOR("vstdlib.dll")
#define VGUI_DLL					XOR("vgui2.dll")
#define VGUIMAT_DLL					XOR("vguimatsurface.dll")
#define MATERIAL_DLL				XOR("materialsystem.dll")
#define LOCALIZE_DLL				XOR("localize.dll")
#define STUDIORENDER_DLL			XOR("studiorender.dll")
#define INPUTSYSTEM_DLL				XOR("inputsystem.dll")
#define SHARED_API					XOR("shaderapidx9.dll")
#define TIER_DLL					XOR("tier0.dll")
#define PANORAMA_DLL				XOR("panorama.dll")
#define FILESYS_DLL					XOR("filesystem_stdio.dll")

// should be close to EntityGlowEffects str
#define GLOWMANAGER					XOR("0F 11 05 ? ? ? ? 83 C8 01")
// const CViewRenderBeams::`vftable should be pushed in:
// #STR #STR: "r_drawbrushmodels", "engine/writez", "debug/debugtranslucentsinglecolor", "engine/modulatesinglecolor", "vgui/white", "r_drawentities", "Other textures"
#define BEAMS						XOR("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")
// #STR: "%s stuck on object %i/%s", "client"
#define MOVEHELPER					XOR("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")
// #STR SelectWeightedSequence
// or never sig it and use exactly code that game uses
#define PREDICTIONRANDOMSEED		XOR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")
// no STR
// it will be easier to find by live debugger
#define SETABSANGLES				XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8")
// Ignoring unreasonable position (%f,%f,%"... is a hint
#define SETABSORIGIN				XOR("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")
// should call check ammo cvar or smth
// #STR: ammo_50AE_impulse
#define WEAPONDATA					XOR("8B 35 ? ? ? ? FF 10 0F B7 C0")
// #STR: "weapon_c4"
#define HASC4						XOR("56 8B F1 85 F6 74 31")
// #STR: (mod_studio)
#define INVALIDATE_BONE_CACHE		XOR("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81")
// #STR: "unknown" - well wow
// resource should also call steamworks thing. Search by this too.
#define PLAYER_RESOURCE				XOR("74 30 8B 35 ? ? ? ? 85 F6")
// useless since in here you can read it can be forced, but it's safer
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/213556-changing-sky-spoofing-cvar.html
#define LOAD_SKY					XOR("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")
// anything with literal named bones/hitboxes
#define ANIMATION_LAYER				XOR("8B 89 ? ? ? ? 8D 0C D1")
// #STR: "No model!", "Not Found!
// and anything with sequence
#define SEQUENCE_ACTIVITY			XOR("55 8B EC 53 8B 5D 08 56 8B F1 83")
// also: 55 8B EC 8B 45 10 50 8B 4D 04, and more... this is for you to figure out. use xrefs...
#define NEW_CHECK					XOR("55 8B EC 56 8B F1 33 C0 57 8B 7D 08 8B 8E")
// #STR: "Video driver has crashed and been reset, re-uploading reso
#define DX9_DEVICE					XOR("A1 ? ? ? ? 50 8B 08 FF 51 0C")
// #STR: DebugView
// #STR: debug/
// finding it with live debugger is easier and I don't think it needs any explanation how to find your local view matrix
#define VIEW_MATRIX_CLIENT			XOR("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")
// #STR: "func_breakable", "func_breakable_surf"
#define IS_BREAKBLE					XOR("55 8B EC 51 56 8B F1 85 F6 74 68")
// this is needed because this convar will rely on many things, the r_drawspecificstaticprop and forcing isn't enough
// to find it use live debugger, or go by some entity save str
#define IS_USING_PROP_DEBUG			XOR("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D")
// see how many pushes are logically to color struct
// should call this one:
// #STR: "CMaterial::PrecacheVars: error loading vmt file for %s\n", "vmt_patches"
#define GET_COLOR_MODULATION		XOR("55 8B EC 83 EC ? 56 8B F1 8A 46")
// disable useless processing on setupbones
// #STR: "ankle_R", "hand_L", "ball_L", "ball_R", "lh_ik_driver", "weapon_hand_R", "ankle_L"
#define EXTRA_BONES_PROCCESSING		XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C 80")
// disable jiggle bones without forcing convar
// better to use live debugger to find it by easier way
#define BUILD_TRANSFORMATIONS		XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 28 8B")
// #STR: "view angles", "Pitch: %6.1f   Yaw: %6.1f %38s", "ideal angles", "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %19s", "camera offset", "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s", "sv_cheats"
#define CAM_THINK					XOR("85 C0 75 30 38 86")
// "#empty#", "#int#"
// also easy to find as 'KeyValuesSystem' should be generated in ida as externs
#define KEY_VALUES_FROM_STR			XOR("55 8B EC 81 EC ? ? ? ? 85 D2 53")
// found by checking what's inside bone threads
// str anything like bone access
#define CACHED_BONE					XOR("FF B7 ? ? ? ? 52")
// #STR: "CParticleCollection::Simulate", "Particle Simulation"
#define PARTICLE_SIMULATE			XOR("55 8B EC 83 E4 F8 83 EC 30 56 57 8B F9 0F 28 E1 8B 0D ? ? ? ? F3 0F 11 64 24 ? 89 7C 24 18 8B 81")
#pragma region dumped_effect_names
/*
ManhackSparks
TeslaZap
TeslaHitboxes
CommandPointer
GunshipImpact
Smoke
MuzzleFlash
Error
BoltImpact
RPGShotDown
GlassImpact
EjectBrass_338Mag
EjectBrass_762Nato
EjectBrass_556
EjectBrass_57
EjectBrass_12Gauge
EjectBrass_9mm
CS_MuzzleFlash_X
CS_MuzzleFlash
KnifeSlash
Impact
CS_HolidayLight
csblood
waterripple
gunshotsplash
watersplashquiet
watersplash
TracerSound
ParticleTracer
Tracer
ShotgunShellEject
RifleShellEject
ShellEject
RagdollImpact
HelicopterMegaBomb
WaterSurfaceExplosion
Explosion
HunterDamage
BloodImpact
bloodspray
WheelDust
ShakeRopes
ParticleEffectStop
ParticleEffect
*/
#pragma endregion
// #STR: "DispatchEffect: effect \"%s\" not found on client\n", "Encountered multiple different effects with the same nam
// see what calls this and you can see head ptr, should be close to actual message
#define HEAD_OF_EFFECTS				XOR("8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F 00")
// check CTraceFilterOmitPlayers
#define GOES_THROUGH_SMOKE			XOR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0")
// #STR: "TempEntity", "TE_DispatchEffect %s %s", "originx", "originy", "originz", "startx", "starty", "startz", "normalx", "normaly"
#define DISPATCH_EFFECT				XOR("55 8B EC 83 E4 F8 83 EC 20 56 57 8B F9 C7 44 24")
// #STR: "effects/tesla_glow_noz"
#define FX_TESLA					XOR("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 47 18")
// #STR: "effects/flashlight001", "Other textures"
#define FLASHLIGHT_CREATE			XOR("55 8B EC F3 0F 10 45 ? B8")
// #STR: "entindex", "flashlightHandle", "flashlightState", "FlashlightState"
#define FLASHLIGHT_DESTROY			XOR("56 8B F1 E8 ? ? ? ? 8B 4E 28")
// #STR: "CFlashlightEffect::UpdateLight", "FlashlightState", "entindex", "flashlightHandle", "flashlightState", "Flashlight Shadows"
#define FLASHLIGHT_UPDATE			XOR("E8 ? ? ? ? 8B 06 F3 0F 10 46")
// possible to not sig it probably
#define CLIENT_STATE				XOR("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")
// no STR
#define GAME_RULES					XOR("8B 0D ? ? ? ? 85 C0 74 0A 8B 01 FF 50 78 83 C0 54")
// STR: "CNetChan_TransmitBits->send", "CNetChan::SendDatagram
#define SEND_DATAGRAM				XOR("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 18")
// STR: const CTraceFilterSimple
#define CTRACE_FILTER_SIMPLE		XOR("55 8B EC 83 E4 F0 83 EC 7C 56 52")
// #STR: "CHudWeaponSelection"
#define CSGO_HUD					XOR("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89")
// #STR: "[%d] Could not find Hud Element: %s\n"
#define FIND_ELEMENT				XOR("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28 7E 1E")
// #STR: "ERROR: model verts have been compressed or you don't have , "Could not render eyeball for model '%s' (it has > 65536 ve, "$glint"
// or find on call stack in x32dbg in studiorender.dll
#define R_STUDIODRAWPOINTS			XOR("8B 7D FC 03 F8")
// to find it you must check it yourself
// anything like Failed to convert image data to RGBA might help to path it
#define UNK_FILESYS					XOR("C7 43 ? ? ? ? ? 83 7C 24 ? ? 7C 25 8B 54 24 48")
// #STR: "Error! CMapOverview::SetMap: couldn't load file %s.\n", "file://{images_overviews}/%s_radar_spectate.dds", "pos_x", "pos_y", "scale", "resource/overviews/%s.txt"
#define UNK_OVERVIEWMAP				XOR("55 8B EC 81 EC ? ? ? ? 53 8B 5D 08 8B D3")
#define VIEW_RENDER					XOR("8B 0D ? ? ? ? FF 75 0C 8B 45 08")
// #STR: "Client_Animation", "C_BaseAnimating::SetupBones", "*** ERROR: Bone access not allowed (entity %i:%s)\n", "%d:%s", "SetupBones: invalid bone array size (%d - needs %d)\n", "Client_Animation_Threaded"
#define ACCUMULATE_LAYERS			XOR("84 C0 75 0D F6 87")
#define SETUP_VELOCITY				XOR("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80")
#define SETUP_OCCLUSION				XOR("84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7")
// #STR: "Tried BeginRenderTargetAllocation after game startup. If I
#define DISABLE_RENDER_TARGET_ALLOC XOR("80 B9 ? ? ? ? ? 74 0F")
// #STR: "userid", "entindex", "weaponhud_selection"
#define LOCAL_PLAYER				XOR("8B 0D ? ? ? ? 83 FF FF 74 07")
// from prediction stack
#define PREDICTION_MOVE_DATA		XOR("A1 ? ? ? ? F3 0F 59 CD")
// #STR: "Client: Missing precache for particle system \"%s\"!\n" - this func is a mess to start working on rain manually
#define DISPATCH_PARTICLE			XOR("55 8B EC 83 E4 F8 83 EC 6C 53 56 57 8B F9 8B DA 8D 4C 24 10 E8")
// see what's under:
// ; #STR: "$c0_x", "_rt_FullFrameFB", "_rt_SmallFB0", "Other textures", "dev/blurgaussian_3x3", "_rt_SmallFB1", "$c0_y", "$c1_x", "$c1_y", "RenderTargets"
// is called
#define IS_DEPTH					XOR("8B 0D ? ? ? ? 56 8B 01 FF 50 34 8B F0 85 F6 75 04")
// wrapper to allow in other hooks
// #STR: "_rt_FullFrameFB", "RenderTargets", "_rt_FullFrameFB%d"
#define DRAW_SPACE_RECTANGLE_CALL	XOR("55 8B EC 83 E4 ? 83 EC ? 53 56 57 8D 44 24 ? 89 4C")