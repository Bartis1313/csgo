#pragma once

#include <array>
#include <Windows.h>

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
    inline constexpr static size_t KEYS_SIZE = 256;
    std::array<KeyState, KEYS_SIZE> m_keys{};
    int m_vKey = 0;
};

inline KeysHandler inputHandler;