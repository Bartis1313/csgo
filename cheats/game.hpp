#pragma once

#include "../SDK/structs/Entity.hpp"

class CUserCmd;
//class Player_t;

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
    _NODISCARD constexpr bool operator==(void* ent) const { return *m_local == ent; }
    // !localplayer
    _NODISCARD constexpr bool operator!() const { return *m_local == nullptr; }
    // get pointer to localplayer
    _NODISCARD constexpr Player_t* operator()() const {return *m_local; }
    // operator for making it like normal pointer
    // possible alternative use: localplayer()->
    // ^ uses no overloaded operator
    _NODISCARD constexpr Player_t* operator->() const { return *m_local; }
private:
    Player_t** m_local;
};

namespace game
{
	inline LocalPlayer localPlayer;
	_NODISCARD bool isAvailable();
	float serverTime(CUserCmd* cmd = nullptr);
}