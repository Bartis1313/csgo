#pragma once

#include <array>
#include <unordered_set>
#include <Windows.h>
#include <initializer_list>
#include <bitset>

class KeysHandler
{
public:
    static void run(UINT message, WPARAM wparam);
    [[nodiscard]] static bool isKeyDown(UINT vKey);
    [[nodiscard]] static bool isKeyPressed(UINT vKey);
    [[nodiscard]] static bool isKeyPressed(const std::initializer_list<UINT>& vKeys);
    [[nodiscard]] static bool isKeyDown(const std::initializer_list<UINT>& vKeys);
private:
    inline constexpr static size_t KEYS_SIZE = 256;
    inline static std::bitset<KEYS_SIZE> m_keyStates;
    inline static std::bitset<KEYS_SIZE> m_keyPressStates;
};