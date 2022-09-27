#pragma once

#include <SDK/structs/Entity.hpp>

class CUserCmd;
struct Studiohdr_t;

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
    inline LocalPlayer localPlayer;
    [[nodiscard]] bool isAvailable();
    float serverTime(CUserCmd* cmd = nullptr);
    [[nodiscard]] uint32_t timeToTicks(float time);
    [[nodiscard]] float ticksToTime(uint32_t ticks);
    [[nodiscard]] float scaleDamageArmor(float dmg, const float armor);
    [[nodiscard]] uintptr_t* findHudElement(const std::string_view name);
    [[nodiscard]] bool isChatOpen();
    [[nodiscard]] float getScaledFont(const Vector& source, const Vector& destination, const float division = 80.0f, const float min = 12.0f, const float max = 30.0f);
    // studio is special case arg - because henade and flashbang have exactly same class id
    // and people who think definition index is ok are wrong, player ents DON'T hold the nade anymore in that case
    [[nodiscard]] WeaponIndex getNadeByClass(int idx, Studiohdr_t* studio);
}