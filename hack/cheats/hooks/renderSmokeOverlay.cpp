#include "renderSmokeOverlay.hpp"

#include "viewFade.hpp"

#include <gamememory/memory.hpp>
#include <SDK/IViewRender.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>

hooks::RenderSmokeOverlay::value hooks::RenderSmokeOverlay::hook(FAST_ARGS, bool enabled)
{
	// edx+588h

	original(thisptr, enabled);
}
