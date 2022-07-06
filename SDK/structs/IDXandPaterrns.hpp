#pragma once

#include "../../utilities/utilities.hpp"

enum VTableIndexes
{
	ISPLAYER = 158,
	ISWEAPON = 166,
	INACCURACY = 483,
	SPREAD = 453,
	WEAPONINFO = 461,
	PUNCH = 346
};

#define ENGINE_DLL				XOR("engine.dll")
#define CLIENT_DLL				XOR("client.dll")
#define VSTD_DLL				XOR("vstdlib.dll")
#define VGUI_DLL				XOR("vgui2.dll")
#define VGUIMAT_DLL				XOR("vguimatsurface.dll")
#define MATERIAL_DLL			XOR("materialsystem.dll")
#define LOCALIZE_DLL			XOR("localize.dll")
#define STUDIORENDER_DLL		XOR("studiorender.dll")
#define INPUTSYSTEM_DLL			XOR("inputsystem.dll")
#define SHARED_API				XOR("shaderapidx9.dll")
#define TIER_DLL				XOR("tier0.dll")
#define PANORAMA_DLL			XOR("panorama.dll")
#define FILESYS_DLL				XOR("filesystem_stdio.dll")

#define GLOWMANAGER				XOR("0F 11 05 ? ? ? ? 83 C8 01")
#define BEAMS					XOR("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")
#define MOVEHELPER				XOR("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")
#define PREDICTIONRANDOMSEED	XOR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")
#define SETABSANGLES			XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8")
#define SETABSORIGIN			XOR("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")
#define WEAPONDATA				XOR("8B 35 ? ? ? ? FF 10 0F B7 C0")
#define HASC4					XOR("56 8B F1 85 F6 74 31")
#define INVALIDATE_BONE_CACHE	XOR("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81")
#define PLAYER_RESOURCE			XOR("74 30 8B 35 ? ? ? ? 85 F6")
// useless since in here you can read it can be forced, but it's safer
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/213556-changing-sky-spoofing-cvar.html
#define LOAD_SKY				XOR("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")
#define ANIMATION_LAYER			XOR("8B 89 ? ? ? ? 8D 0C D1")
#define SEQUENCE_ACTIVITY		XOR("55 8B EC 53 8B 5D 08 56 8B F1 83")
#define NEW_CHECK				XOR("55 8B EC 56 8B F1 33 C0 57 8B 7D 08 8B 8E") // direct sig, sub_10797090
#define DX9_DEVICE				XOR("A1 ? ? ? ? 50 8B 08 FF 51 0C")
#define VIEW_MATRIX_CLIENT		XOR("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")
#define IS_BREAKBLE				XOR("55 8B EC 51 56 8B F1 85 F6 74 68")
// this is needed because this convar will rely on many things, the r_drawspecificstaticprop and forcing isn't enough
#define IS_USING_PROP_DEBUG		XOR("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D")
#define GET_COLOR_MODULATION	XOR("55 8B EC 83 EC ? 56 8B F1 8A 46")
// disable useless processing on setupbones
#define EXTRA_BONES_PROCCESSING	XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C 80")
// disable jiggle bones without forcing convar
#define BUILD_TRANSFORMATIONS	XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 28 8B")
#define CAM_THINK				XOR("85 C0 75 30 38 86")
// chams
#define KEY_VALUES_FROM_STR		XOR("55 8B EC 81 EC ? ? ? ? 85 D2 53")
#define CACHED_BONE				XOR("FF B7 ? ? ? ? 52")
#define PARTICLE_SIMULATE		XOR("55 8B EC 83 E4 F8 83 EC 30 56 57 8B F9 0F 28 E1 8B 0D ? ? ? ? F3 0F 11 64 24 ? 89 7C 24 18 8B 81")
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
#define HEAD_OF_EFFECTS			XOR("8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F 00")
#define GOES_THROUGH_SMOKE		XOR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0")
#define DISPATCH_EFFECT			XOR("55 8B EC 83 E4 F8 83 EC 20 56 57 8B F9 C7 44 24")
#define FX_TESLA				XOR("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 47 18")
#define FLASHLIGHT_CREATE		XOR("55 8B EC F3 0F 10 45 ? B8")
#define FLASHLIGHT_DESTROY		XOR("56 8B F1 E8 ? ? ? ? 8B 4E 28")
#define FLASHLIGHT_UPDATE		XOR("E8 ? ? ? ? 8B 06 F3 0F 10 46")
#define CLIENT_STATE			XOR("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")
#define GAME_RULES				XOR("8B EC 8B 0D ? ? ? ? 85 C9 74 07")
#define SEND_DATAGRAM			XOR("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 18")
#define CTRACE_FILTER_SIMPLE	XOR("55 8B EC 83 E4 F0 83 EC 7C 56 52")
#define CSGO_HUD				XOR("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89")
#define FIND_ELEMENT			XOR("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28 7E 1E")
// #STR: "ERROR: model verts have been compressed or you don't have , "Could not render eyeball for model '%s' (it has > 65536 ve, "$glint"
// or find on call stack in x32dbg in studiorender.dll
#define R_STUDIODRAWPOINTS		XOR("8B 7D FC 03 F8")
#define UNK_FILESYS				XOR("C7 43 ? ? ? ? ? 83 7C 24 ? ? 7C 25 8B 54 24 48")
// #STR: "Error! CMapOverview::SetMap: couldn't load file %s.\n", "file://{images_overviews}/%s_radar_spectate.dds", "pos_x", "pos_y", "scale", "resource/overviews/%s.txt"
#define UNK_OVBERVIEWMAP		XOR("55 8B EC 81 EC ? ? ? ? 53 8B 5D 08 8B D3")