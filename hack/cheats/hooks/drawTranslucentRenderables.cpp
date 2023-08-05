#include "drawTranslucentRenderables.hpp"

#include <cheats/features/visuals/chams/chams.hpp>
#include <config/vars.hpp>

static void renderChams()
{
	chams::streamProof.setActive(vars::visuals->chams->streamProof);
	chams::streamProof.beginMaterialHook();
	chams::manual::run();
	chams::streamProof.endMaterialHook();
}

hooks::DrawTranslucentRenderables::value hooks::DrawTranslucentRenderables::hook(HACK_FAST_ARGS, bool inSkybox, bool shadowDepth)
{
	const bool renderThroughSmoke = vars::visuals->chams->ignoreSmoke;

	if (!renderThroughSmoke)
	{
		renderChams();
		original(thisptr, inSkybox, shadowDepth);
	}
	else
	{
		original(thisptr, inSkybox, shadowDepth);
		renderChams();
	}
}