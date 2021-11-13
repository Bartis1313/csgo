#include "hooks.hpp"
#include "../menu/menu.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	original(interfaces::panel, panel, forceRepaint, allowForce);

	const auto toDraw = interfaces::panel->getName(panel);

	if (strstr(toDraw, XOR("MatSystemTopPanel")))
	{
		Menu::g().draw();
		Menu::g().handleKeys(vars::options);
		esp::run();
		world::drawMisc();
		radar::run();
		misc::drawCrosshair();
		misc::drawLocalInfo();
	}
}