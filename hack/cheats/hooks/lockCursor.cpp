#include "lockCursor.hpp"

#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>

hooks::LockCursor::value hooks::LockCursor::hook(HACK_FAST_ARGS)
{
	if (ImGuiMenu::active)
	{
		memory::interfaces::surface->unlockCursor();
		return;
	}

	original(thisptr);
}