#pragma once

#include <array>
#include <Windows.h>

#define KEYS_SIZE 256

enum class KeyState
{
    OFF,
    DOWN,
    UP,
    PRESS
};

// eliminates using GetAsyncKeyState
class KeysHandler
{
public:
    KeysHandler() = default;
    void run(UINT message, WPARAM wparam);
    _NODISCARD bool isKeyDown(UINT vKey) const;
    _NODISCARD bool isKeyPressed(UINT vKey);
    _NODISCARD constexpr int getVKey() const { return m_vKey; }
private:
    std::array<KeyState, KEYS_SIZE> m_keys;
    int m_vKey = 0;
};

inline KeysHandler inputHandler;