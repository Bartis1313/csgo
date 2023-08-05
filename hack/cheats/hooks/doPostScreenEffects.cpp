#include "doPostScreenEffects.hpp"

#include <cheats/features/visuals/streamproof/streamproof.hpp>
#include <cheats/features/visuals/glow/glow.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>
#include "viewFade.hpp"
#include "particlesSimulation.hpp"

#include <SDK/IViewRender.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/material.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/CParticelCollection.hpp>
#include <cheats/features/visuals/glow/glow.hpp>
#include <gamememory/memory.hpp>

hooks::DoPostScreenEffects::value hooks::DoPostScreenEffects::hook(HACK_FAST_ARGS, int val)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(thisptr, val);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });
	
	const auto ret = original(thisptr, val);

	glow::streamProof.setActive(game::isAvailable() && vars::visuals->glow->streamProof);

	glow::streamProof.beginMaterialHook();

	Storage::runs.run();

	glow::streamProof.endMaterialHook();
	return ret;
}