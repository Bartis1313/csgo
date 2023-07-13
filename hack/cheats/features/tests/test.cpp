#include "test.hpp"

#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <cheats/game/game.hpp>
#include <cheats/features/particle/particle.hpp>
#include <utilities/inputSystem.hpp>

#include <gamememory/memory.hpp>
#include <cheats/hooks/frameStageNotify.hpp>

// place anything you want to test

namespace
{
	struct WeatherHandler : hooks::FrameStageNotify
	{
		WeatherHandler()
		{
			this->registerRun(test::run);
		}
	} weatherHandler;
}

void test::run(FrameStage stage)
{
	if (!game::isAvailable())
		return;

	if (inputSystem::isKeyDown(VK_NUMPAD1))
	{
		Particle::dispatchParticle("water_splash_01_surface2", game::localPlayer()->absOrigin());
	}

	static auto mat = memory::interfaces::matSys->findMaterial("particle/rain_streak", TEXTURE_GROUP_CLIENT_EFFECTS);
	//mat->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
}