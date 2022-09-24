#include "hooks.hpp"

#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>

void __stdcall hooks::lockCursor::hooked()
{
	if (menu.isMenuActive())
	{
		interfaces::surface->unlockCursor();
		return;
	}

	original(interfaces::surface);
}