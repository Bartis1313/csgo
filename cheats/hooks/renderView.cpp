#include "hooks.hpp"

#include "../classes/renderView.hpp"

#include <SDK/CViewSetup.hpp>

void FASTCALL hooks::renderView::hooked(FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw)
{
	original(thisptr, view, hud, clearFlags, whatToDraw);

	RenderViewType::runAll(view);
}