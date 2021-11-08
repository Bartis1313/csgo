#pragma once

// 01:49 08.11.2021
// Bartis: updated

enum VTableIndexes
{
	ISPLAYER = 158,
	ISWEAPON = 166,
	INACCURACY = 483,
	SPREAD = 453,
	WEAPONINFO = 461,
	PUNCH = 346
};

// #define SIG(name, pattern) const char* SIG_ ## name = pattern

#define ENGINE_DLL			XOR("engine.dll")
#define CLIENT_DLL			XOR("client.dll")
#define VSTD_DLL			XOR("vstdlib.dll")
#define VGUI_DLL			XOR("vgui2.dll")
#define VGUIMAT_DLL			XOR("vguimatsurface.dll")
#define MATERIAL_DLL		XOR("materialsystem.dll")
#define LOCALIZE_DLL		XOR("localize.dll")
#define STUDIORENDER_DLL	XOR("studiorender.dll")

#define GLOWMANAGER				XOR("0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? 00 00 00 00")
#define BEAMS					XOR("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9")
#define MOVEHELPER				XOR("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")
#define PREDICTIONRANDOMSEED	XOR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")
#define SETABSANGLES			XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8")
#define SETABSORIGIN			XOR("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")
#define WEAPONDATA				XOR("8B 35 ? ? ? ? FF 10 0F B7 C0")
#define HASC4					XOR("56 8B F1 85 F6 74 31")
#define INVALIDATE_BONE_CACHE	XOR("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81")
#define PLAYER_RESOURCE			XOR("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7")