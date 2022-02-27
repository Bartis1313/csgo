#include "hooks.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../globals.hpp"
#include "../../config/vars.hpp"

#pragma region "Paint Helpers"
bool isValidWindow()
{
	// sub window is better, for cs as they recently updated main window name
#ifdef _DEBUG
	if (auto window = FindWindowA("Valve001", NULL); GetForegroundWindow() != window)
		return false;
#else
	if (auto window = LF(FindWindowA).cached()(XOR("Valve001"), NULL); LF(GetForegroundWindow).cached()() != window)
		return false;
#endif
	return true;
}

// run current screensize
void getScreen()
{
	int x, y;
	interfaces::engine->getScreenSize(x, y);
	globals::screenX = x;
	globals::screenY = y;
}

// draw all painting
void paintHandle()
{
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
	legitbot::drawFov();
}

#pragma endregion

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	getScreen();

	if (interfaces::engine->isTakingScreenshot())
		return;

	static unsigned int panelScope = 0;

	const auto panelName = interfaces::panel->getName(panel);

	if (!panelScope)
	{
		if(strstr(panelName, XOR("HudZoom")))
			panelScope = panel;
	}
	else if (panelScope == panel)
	{
		if (interfaces::engine->isInGame() && config.get<bool>(vars.bNoScope))
			return;
	}

	original(interfaces::panel, panel, forceRepaint, allowForce);
}