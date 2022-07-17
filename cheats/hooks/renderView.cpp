#include "hooks.hpp"

#include "../../SDK/CViewSetup.hpp"

#include "../features/visuals/mirrorCam.hpp"

void __fastcall hooks::renderView::hooked(FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw)
{
	mCam.renderCustomView(view);

 	original(thisptr, view, hud, clearFlags, whatToDraw);
}