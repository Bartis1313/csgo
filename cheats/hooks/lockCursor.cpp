#include "hooks.hpp"

#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>

void __fastcall hooks::lockCursor::hooked(FAST_ARGS)
{
	if (menu.isMenuActive())
	{
		interfaces::surface->unlockCursor();
		return;
	}

	original(thisptr);
}