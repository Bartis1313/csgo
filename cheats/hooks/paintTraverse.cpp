#include "hooks.hpp"
#include "../menu/menu.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../menu/GUI/gui.hpp"
#include "../globals.hpp"

#pragma region "Paint Helpers"
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

bool isValidWindow()
{
	// sub window is better, for cs as they recently updated main window name
	if (auto window = LF(FindWindowW).cached()(XOR(L"Valve001"), NULL); LF(GetForegroundWindow)() != window)
		return false;

	return true;
}
#pragma endregion

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	if (!isValidWindow())
		return;

	interfaces::surface->getCursor(globals::mouseX, globals::mouseY);

	// will run first no matter what, you can edit the function a bit or add ghetto static counter
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