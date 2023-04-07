#pragma once

#include <SDK/structs/Entity.hpp>
#include <gamememory/memory.hpp>

class CUserCmd;
struct Studiohdr_t;
struct IDirect3DTexture9;
using LPDIRECT3DTEXTURE9 = IDirect3DTexture9*;

class LocalPlayer final
{
public:
	// 
	constexpr LocalPlayer()
		: m_local{ nullptr }
	{}
	// do it once only
	void init();
	// ent == localplayer
	[[nodiscard]] constexpr bool operator==(void* ent) const { return *m_local == ent; }
	// !localplayer
	[[nodiscard]] constexpr bool operator!() const { return *m_local == nullptr; }
	// get pointer to localplayer
	[[nodiscard]] constexpr Player_t* operator()() const { return *m_local; }
	// operator for making it like normal pointer
	// possible alternative use: localplayer()->
	// ^ uses no overloaded operator
	[[nodiscard]] constexpr Player_t* operator->() const { return *m_local; }
private:
	Player_t** m_local;
};

namespace game
{
	struct BitMapData
	{
		float width, height;
		LPDIRECT3DTEXTURE9 texture;
	};

	inline LocalPlayer localPlayer;
	[[nodiscard]] bool isAvailable();
	float serverTime(CUserCmd* cmd = nullptr);
	[[nodiscard]] uint32_t timeToTicks(float time);
	[[nodiscard]] float ticksToTime(uint32_t ticks);
	[[nodiscard]] float scaleDamageArmor(float dmg, const float armor);
	template<typename T = uintptr_t*>
	[[nodiscard]] T findHudElement(const std::string_view name);
	[[nodiscard]] bool isChatOpen();
	[[nodiscard]] float getScaledFont(const Vec3& source, const Vec3& destination, const float division = 80.0f, const float min = 12.0f, const float max = 30.0f);
	// studio is special case arg - because henade and flashbang have exactly same class id
	// and people who think definition index is ok are wrong, player ents DON'T hold the nade anymore in that case
	[[nodiscard]] WeaponIndex getNadeByClass(int idx, Studiohdr_t* studio);
	[[nodiscard]] Vec3 getViewAngles();
	// lerped and decayed punch
	[[nodiscard]] Vec3 getFixedPunch();
	[[nodiscard]] BitMapData getWeaponIcon(const std::string_view weapon);
	// not for svg, string_view is not passed due to high possibility of using std::format
	// eg path: resource/flash/econ/weapons/base_weapons/ + weapon name + .png
	[[nodiscard]] BitMapData getImage(const std::string& path);
}

template<typename T>
T game::findHudElement(const std::string_view name)
{
	return reinterpret_cast<T>(memory::hudfindElement()(memory::csgoHud(), name.data()));
}