#include "hooks.hpp"

#include "../classes/renderableToSurface.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IPanel.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <utilities/res.hpp>
#include <utilities/tools/tools.hpp>

static void getScreen()
{
	int x, y;
	memory::interfaces::engine->getScreenSize(x, y);
	globals::screenX = x;
	globals::screenY = y;
}

static void getMouse()
{
	int x, y;
	memory::interfaces::surface->getCursor(x, y);
	globals::mouseX = x;
	globals::mouseY = y;
}

hooks::paintTraverse::value FASTCALL hooks::paintTraverse::hooked(FAST_ARGS, unsigned int panel, bool forceRepaint, bool allowForce)
{
	getScreen();
	getMouse();

	if (memory::interfaces::engine->isTakingScreenshot())
		return;

	const std::string_view panelName = memory::interfaces::panel->getName(panel);

	static unsigned int panelScope = 0;
	static unsigned int panelID = 0;

	if (!panelScope)
	{
		if (panelName == "HudZoom")
			panelScope = panel;
	}
	else if (panelScope == panel)
	{
		if (memory::interfaces::engine->isInGame() && vars::misc->scope->enabled)
			return;
	}

	original(thisptr, panel, forceRepaint, allowForce);

	//static Resource res{ IDB_PNG1, "PNG" };

	if (!panelID)
	{
		if (panelName == "FocusOverlayPanel")
			panelID = panel;
	}
	else if (panelID == panel)
	{
		imRender.addToRender([]()
			{
				RenderableSurfaceType::runAll();
			});

		x88menu.draw();
	}
}