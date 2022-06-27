#include "hooks.hpp"

#include "../../SDK/IVEngineClient.hpp"
#include "../../SDK/IPanel.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../features/prediction/nadepred.hpp"

#include "../globals.hpp"
#include "../../config/vars.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../menu/x88Menu/x88menu.hpp"
#include "../features/misc/logger.hpp"
#include "../../utilities/res.hpp"
#include "../../resource.h"

#pragma region "Paint Helpers"
// run current screensize
static void getScreen()
{
	int x, y;
	interfaces::engine->getScreenSize(x, y);
	globals::screenX = x;
	globals::screenY = y;
}

static void getMouse()
{
	int x, y;
	interfaces::surface->getCursor(x, y);
	globals::mouseX = x;
	globals::mouseY = y;
}
#pragma endregion

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	getScreen();
	getMouse();

	if (interfaces::engine->isTakingScreenshot())
		return;

	const std::string_view panelName = interfaces::panel->getName(panel);

	static unsigned int panelScope = 0;
	static unsigned int panelID = 0;

	if (!panelScope)
	{
		if (panelName == XOR("HudZoom"))
			panelScope = panel;
	}
	else if (panelScope == panel)
	{
		if (interfaces::engine->isInGame() && config.get<bool>(vars.bNoScope))
			return;
	}

	original(interfaces::panel, panel, forceRepaint, allowForce);

	//static Resource res{ IDB_PNG1, "PNG" };

	if (!panelID)
	{
		if (panelName == XOR("FocusOverlayPanel"))
			panelID = panel;
	}
	else if (panelID == panel)
	{
		imRender.addToRender([]()
			{
				visuals.run();
				world.drawMisc();
				misc.drawLocalInfo();
				//misc.drawFpsPlot();
				//misc.drawVelocityPlot();
				misc.drawHitmarker();
				world.drawZeusRange();
				misc.drawNoScope();
				misc.drawCrosshair();
				aimbot.draw();
				world.drawMovementTrail();
				nadePred.draw();
				misc.drawHat();
				world.clientSideImpacts();
				world.localImpacts();
				logger.draw();
			});

		x88menu.draw();
	}
}