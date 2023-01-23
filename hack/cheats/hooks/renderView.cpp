#include "hooks.hpp"

#include "../classes/viewRender.hpp"

#include <SDK/CViewSetup.hpp>

hooks::viewRender::value FASTCALL hooks::viewRender::hooked(FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw)
{
	ViewRenderType::runAll(view);

	original(thisptr, view, hud, clearFlags, whatToDraw);
}