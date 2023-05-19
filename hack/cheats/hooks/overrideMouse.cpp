#include "overrideMouse.hpp"

#include <menu/GUI-ImGui/menu.hpp>

hooks::OverrideMouse::value hooks::OverrideMouse::hook(FAST_ARGS, float* x, float* y)
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::intis.run(); });

	if (ImGuiMenu::active)
		return original(thisptr, x, y);

	original(thisptr, x, y);

	Storage::runs.run(x, y);
}