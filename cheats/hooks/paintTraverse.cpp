#include "hooks.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../globals.hpp"
#include "../../config/vars.hpp"

#pragma region "Paint Helpers"
// all paint
void paintHandle()
{
	esp::run();
	world::drawMisc();
	misc::drawLocalInfo();
	misc::drawFpsPlot();
	misc::drawVelocityPlot();
	misc::drawHitmarker();
	world::drawZeusRange();
	misc::drawNoScope();
	misc::drawCrosshair();
	legitbot::drawFov();
	world::drawMovementTrail();
}

// run current screensize
void getScreen()
{
	int x, y;
	interfaces::engine->getScreenSize(x, y);
	globals::screenX = x;
	globals::screenY = y;
}
#pragma endregion

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	//if (!utilities::isValidWindow()) // should check it less
		//return;

	getScreen();

	if (interfaces::engine->isTakingScreenshot())
		return;

	static unsigned int panelScope = 0;
	static unsigned int panelID = 0;

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

	if (!panelID)
	{
		if (strstr(panelName, XOR("MatSystemTopPanel")))
			panelID = panel;
	}
	else if (panelID == panel)
	{
		imRender.clearData();
		imRender.addToRender(&paintHandle);
		imRender.swapData();
	}
}