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
    bool isKeyDown(UINT vKey) const;
    bool isKeyPressed(UINT vKey);
private:
    std::array<KeyState, KEYS_SIZE> m_keys;
};

inline KeysHandler inputHandler;