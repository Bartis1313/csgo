#pragma once

#include <vector>
#include <string>
#include <utility>

#include "../utilities/utilities.hpp"

enum class KeyMode : size_t
{
	DOWN,
	TOGGLE,
	KEYMODE_SIZE
};

class Key
{
public:
	Key() = default;
	constexpr Key(KeyMode mode, int code)
		: m_keyMode{ mode }, m_keyCode{ code }
	{}
	// use only for simple keys for features etc...
	// if class contains an "active" field then this will be a problem
	void update();
	bool isEnabled() const;
	// to hotkey
	bool checkKey();
	bool isPressed() const;
	bool isDown() const;
	bool isToggled() const;
	int getKeyCode() const { return m_keyCode; }
	KeyMode getKeyMode() const { return m_keyMode; }
	void setKeyMode(KeyMode mode) { m_keyMode = mode; }
	static std::array<std::pair<KeyMode, const char*>, E2T(KeyMode::KEYMODE_SIZE)> getKeyPairs();
private:
	bool m_toggle = false;
	int m_keyCode;
	KeyMode m_keyMode;
};