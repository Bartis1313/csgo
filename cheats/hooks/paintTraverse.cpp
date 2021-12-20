#include "hooks.hpp"
#include "../menu/menu.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../menu/GUI/gui.hpp"

bool shouldReloadsFonts()
{
	static int oldX, oldY, x, y;
	interfaces::engine->getScreenSize(x, y);

	if (x != oldX || y != oldY)
	{
		oldX = x;
		oldY = y;
		return true;
	}

	return false;
}

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	if (shouldReloadsFonts())
		render::init();

	if (strstr(interfaces::panel->getName(panel), XOR("HudZoom")))
	{
		if (interfaces::engine->isInGame())		
			return;
	}

	original(interfaces::panel, panel, forceRepaint, allowForce);

	if (strstr(interfaces::panel->getName(panel), XOR("MatSystemTopPanel")))
	{
		Menu::g().draw();
		Menu::g().handleKeys();
		esp::run();
		world::drawMisc();
		radar::run();
		misc::drawLocalInfo();
		misc::drawFpsPlot();
		misc::drawVelocityPlot();
		misc::drawHitmarker();
		world::drawZeusRange();
		misc::drawNoScope();
		misc::drawCrosshair();
		// testing image, all good
		test::run();
	}
}