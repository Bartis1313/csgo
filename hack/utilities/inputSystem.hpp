#pragma once

#include <array>
#include <unordered_set>
#include <Windows.h>
#include <initializer_list>
#include <bitset>

class KeysHandler
{
public:
    KeysHandler() = default;
    void run(UINT message, WPARAM wparam);
    [[nodiscard]] bool isKeyDown(UINT vKey) const;
    [[nodiscard]] bool isKeyPressed(UINT vKey) const;
    [[nodiscard]] bool isKeyPressed(const std::initializer_list<UINT>& vKeys) const;
    [[nodiscard]] bool isKeyDown(const std::initializer_list<UINT>& vKeys) const;
private:
    inline constexpr static size_t KEYS_SIZE = 256;
    std::bitset<KEYS_SIZE> m_keyStates;
    std::bitset<KEYS_SIZE> m_keyPressStates;
    int m_vKey = 0;
};

inline KeysHandler inputHandler;