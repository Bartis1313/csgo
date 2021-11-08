#include "hooks.hpp"
#include "../menu/menu.hpp"
#include "../features/visuals/visuals.hpp"
#include "../features/aimbot/aimbot.hpp"

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	original(interfaces::panel, panel, forceRepaint, allowForce);

	const auto toDraw = interfaces::panel->getName(panel);

	if (strstr(toDraw, XOR("MatSystemTopPanel")))
	{
		Menu::g().draw();
		Esp::draw();
		Esp::drawCrosshair();
		Esp::radar();
		Esp::drawLocalInfo();
	}
}