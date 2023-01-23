#include "hooks.hpp"

#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>

hooks::lockCursor::value FASTCALL hooks::lockCursor::hooked(FAST_ARGS)
{
	if (menu.isMenuActive())
	{
		memory::interfaces::surface->unlockCursor();
		return;
	}

	original(thisptr);
}