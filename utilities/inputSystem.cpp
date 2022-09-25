#include "inputSystem.hpp"

#include <game/globals.hpp>
#include <game/game.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>

void KeysHandler::run(UINT message, WPARAM wparam)
{
	if (globals::isInHotkey)
		return;

	if (game::isChatOpen() || interfaces::engine->isConsoleVisible())
		return;

	// init starting keys, undefined
	m_vKey = 0;
	auto state = KeyState::OFF;

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (wparam < KEYS_SIZE)
		{
			m_vKey = wparam;
			state = KeyState::DOWN;
		}
	}
	break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (wparam < KEYS_SIZE)
		{
			m_vKey = wparam;
			state = KeyState::UP;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		m_vKey = VK_LBUTTON;
		state = message == WM_LBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	{
		m_vKey = VK_RBUTTON;
		state = message == WM_RBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	{
		m_vKey = VK_MBUTTON;
		state = message == WM_MBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
	{
		m_vKey = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		state = message == WM_XBUTTONUP ? KeyState::UP : KeyState::DOWN;
	}
	break;
	default:
		break;
	}

	// save the key
	if (state == KeyState::UP && m_keys.at(m_vKey) == KeyState::DOWN)
		m_keys.at(m_vKey) = KeyState::PRESS;
	else
		m_keys.at(m_vKey) = state;
}

bool KeysHandler::isKeyDown(UINT vKey) const
{
	return m_keys.at(vKey) == KeyState::DOWN;
}

bool KeysHandler::isKeyPressed(UINT vKey)
{
	if (m_keys.at(vKey) == KeyState::PRESS)
	{
		m_keys.at(vKey) = KeyState::UP;
		return true;
	}

	return false;
}