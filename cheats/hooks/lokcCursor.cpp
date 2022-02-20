#include "hooks.hpp"
#include "../menu/GUI-ImGui/menu.hpp"

void __stdcall hooks::lockCursor::hooked()
{
	if (ImGuiMenu::isMenuActive)
	{
		interfaces::surface->unlockCursor();
		return;
	}

	original(interfaces::surface);
}