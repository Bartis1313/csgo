#include "paintTraverse.hpp"

#include <render/render.hpp>
#include <gamememory/memory.hpp>

#include <SDK/IVEngineClient.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/IPanel.hpp>
#include <SDK/interfaces/interfaces.hpp>

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

hooks::PaintTraverse::value hooks::PaintTraverse::hook(HACK_FAST_ARGS, uint32_t panel, bool forceRepaint, bool allowForce)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(thisptr, panel, forceRepaint, allowForce);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	getScreen();
	getMouse();

	if (memory::interfaces::engine->isTakingScreenshot())
		return;

	const std::string_view panelName = memory::interfaces::panel->getName(panel);
	
	static uint32_t panelID = 0;

	original(thisptr, panel, forceRepaint, allowForce);

	if (!panelID)
	{
		if (panelName == "FocusOverlayPanel")
			panelID = panel;
	}
	else if (panelID == panel)
	{
		memory::interfaces::surface->pushMakeCurrent(panel, false);
		ImRender::beginThink();
		Storage::runs.run();
		ImRender::endThink();
		memory::interfaces::surface->popMakeCurrent(panel);
	}
}