#include "key.hpp"

#include <Windows.h>
#include <array>

#include <utilities/inputSystem.hpp>

#include <imgui.h>

// should pass to hotkey by some callback or array arg
constexpr std::array badKeys =
{
	0x5B, 0x5C
};

// used only in menu so no need to care for own input handler here
bool Key::checkKey()
{
	for (const auto el : badKeys)
	{
		if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(el)))
			return true;
	}
	// imgui has many key handlers, so mouse is not treated like keyboard
	// 0=left, 1=right, 2=middle + extras (ImGuiMouseButton_COUNT == 5)
	for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); i++)
	{
		if (ImGui::IsMouseClicked(i))
		{
			// need this switch because imgui has own bool arr
			switch (i)
			{
			case 0:
				m_keyCode = VK_LBUTTON;
				break;
			case 1:
				m_keyCode = VK_RBUTTON;
				break;
			case 2:
				m_keyCode = VK_MBUTTON;
				break;
			case 3:
				m_keyCode = VK_XBUTTON1;
				break;
			case 4:
				m_keyCode = VK_XBUTTON2;
				break;
			default:
				break;
			}

			return true;
		}

	}

	for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); i++)
	{
		if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(i)))
		{

			m_keyCode = i;
			return true;
		}
	}

	return false;
}

// for mouse events
//static constexpr int keyToImKey(int id)
//{
//    switch (id)
//    {
//    case VK_LBUTTON:
//        return 0;
//    case VK_RBUTTON:
//        return 1;
//    case VK_MBUTTON:
//        return 2;
//    case VK_XBUTTON1:
//        return 3;
//    case VK_XBUTTON2:
//        return 4;
//    default:
//        return -1;
//    }
//}

void Key::update()
{
	if (m_keyMode != KeyMode::TOGGLE)
		return;

	if (isPressed())
		m_toggle = !m_toggle;
}

bool Key::isPressed() const
{
	return KeysHandler::isKeyPressed(m_keyCode);
}

bool Key::isDown() const
{
	/*auto msk = keyToImKey(m_keyCode);

	if (msk >= 0 && msk <= 4)
		return ImGui::IsMouseDown(msk);*/

	return KeysHandler::isKeyDown(m_keyCode);
}

bool Key::isToggled() const
{
	return m_toggle;
}

bool Key::isEnabled() const
{
	switch (m_keyMode)
	{
	case KeyMode::DOWN:
		return isDown();
	case KeyMode::TOGGLE:
		return isToggled();
	default:
		break;
	}
	return false;
}

std::array<std::pair<KeyMode, const char*>, E2T(KeyMode::KEYMODE_SIZE)> Key::getKeyPairs()
{
	static std::array p =
	{
		std::make_pair(KeyMode::DOWN, "DOWN"),
		std::make_pair(KeyMode::TOGGLE, "TOGGLE")
	};
	return p;
}
