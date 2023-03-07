#include "inputSystem.hpp"

#include <cheats/game/globals.hpp>
#include <cheats/game/game.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>

enum class KeyState
{
	OFF,
	DOWN,
	UP,
	PRESS
};

void KeysHandler::run(UINT message, WPARAM wparam)
{
	if (game::isChatOpen() || memory::interfaces::engine->isConsoleVisible())
		return;

	// init starting keys, undefined
	UINT key = 0;
	auto state = KeyState::OFF;

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (wparam < KEYS_SIZE)
		{
			key = wparam;
			state = KeyState::DOWN;
		}
	}
	break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (wparam < KEYS_SIZE)
		{
			key = wparam;
			state = KeyState::UP;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		key = VK_LBUTTON;
		state = message == WM_LBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	{
		key = VK_RBUTTON;
		state = message == WM_RBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	{
		key = VK_MBUTTON;
		state = message == WM_MBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
	{
		key = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		state = message == WM_XBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	default:
		break;
	}

	// save the key
	if (state == KeyState::DOWN)
	{
		m_keyStates.set(key);
	}
	else if (state == KeyState::UP)
	{
		m_keyStates.reset(key);
		m_keyPressStates.set(key);
	}
}

bool KeysHandler::isKeyDown(UINT vKey)
{
	return m_keyStates[vKey];
}

bool KeysHandler::isKeyPressed(UINT vKey)
{
	if (m_keyPressStates[vKey])
	{
		m_keyPressStates.reset(vKey);
		return true;
	}
	return false;
}

bool KeysHandler::isKeyDown(const std::initializer_list<UINT>& vKeys)
{
	for (const auto vKey : vKeys)
	{
		if (!m_keyStates[vKey])
			return false;
	}
	return true;
}

bool KeysHandler::isKeyPressed(const std::initializer_list<UINT>& vKeys)
{
	for (const auto vKey : vKeys)
	{
		if (!m_keyPressStates[vKey])
			return false;
	}

	for (const auto vKey : vKeys)
	{
		m_keyPressStates.reset(vKey);
	}
	return true;
}