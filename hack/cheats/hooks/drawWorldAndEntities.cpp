#include "drawWorldAndEntities.hpp"

#include "particlesSimulation.hpp"

#include <cheats/features/visuals/chams/chams.hpp>

hooks::DrawWorldAndEntities::value hooks::DrawWorldAndEntities::hook(HACK_FAST_ARGS, bool drawSkybox, const CViewSetup& viewIn, int clearFlags, void* customVisibility)
{
	original(thisptr, drawSkybox, viewIn, clearFlags, customVisibility);
}