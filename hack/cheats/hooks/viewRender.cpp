#include "viewRender.hpp"

hooks::ViewRender::value hooks::ViewRender::hook(FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw)
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	original(thisptr, view, hud, clearFlags, whatToDraw);

	Storage::runs.run(view);
}