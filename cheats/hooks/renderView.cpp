#include "hooks.hpp"

#include "../classes/renderView.hpp"

#include <SDK/CViewSetup.hpp>

void __fastcall hooks::renderView::hooked(FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw)
{
	RenderViewType::runAll(view);

	original(thisptr, view, hud, clearFlags, whatToDraw);
}