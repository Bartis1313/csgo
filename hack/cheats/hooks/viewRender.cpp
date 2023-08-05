#include "viewRender.hpp"

#include <cheats/features/visuals/glow/glow.hpp>
#include <gamememory/memory.hpp>

hooks::ViewRender::value hooks::ViewRender::hook(HACK_FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw)
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	Storage::runs.run(view);

	original(thisptr, view, hud, clearFlags, whatToDraw);
}