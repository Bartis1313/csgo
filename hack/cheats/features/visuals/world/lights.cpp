#include "lights.hpp"

#include <SDK/CCommonHostState.hpp>
#include <SDK/IVEffects.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/material.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <cheats/game/game.hpp>
#include <config/vars.hpp>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/levelInitPostEnttity.hpp>

namespace
{
	struct LightsHandler : hooks::FrameStageNotify
	{
		LightsHandler()
		{
			this->registerInit(lights::init);
			this->registerRun(lights::run);
			this->registerShutdown(lights::shutdown);
		}
	} lightsHandler;

	struct LightReset : hooks::LevelInitPostEntity
	{
		LightReset()
		{
			this->registerPostReset(lights::postReset);
		}
	} lightReset;
}

namespace lights
{
	std::vector<Vec3> savedColors{ };
	IConVar* r_decals{ };
}

void lights::init()
{
	r_decals = memory::interfaces::cvar->findVar("r_decals");
}

void lights::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	if (savedColors.empty())
		postReset();

	for (int i = 0; i < memory::interfaces::hostState()->worldbrush.numworldlights; ++i)
	{
		dworldlight_t* light = &memory::interfaces::hostState()->worldbrush.worldlights[i];
		const emittype_t type = light->type;

		if (!vars::visuals->world->modulate->enabledLights.at(type))
			continue;
	
		light->intensity = Vec3{ vars::visuals->world->modulate->lights.at(type)().getRGB()} *vars::visuals->world->modulate->lightsStrenght.at(type);
	}

	const int maxDecals = std::max(64, r_decals->getInt());
	int counter = 0;

	/*for (int i = 0; i < maxDecals; ++i)
	{
		decal_t* decal = memory::decalPool()[i];

		if (decal)
		{
			if (decal->material)
			{
				const std::string_view name = decal->material->getName();
				if (name.contains("blood"))
					printf("colors [%i %i %i %i]\n", decal->color.r, decal->color.g, decal->color.b, decal->color.a);

				printf("decal material name %s\n", name.data());
			}
		}
	}*/
}

void lights::postReset()
{
	savedColors.clear();

	for (int i = 0; i < memory::interfaces::hostState->worldbrush.numworldlights; ++i)
	{
		dworldlight_t* light = &memory::interfaces::hostState->worldbrush.worldlights[i];
		const emittype_t type = light->type;

		savedColors.emplace_back(light->intensity);
	}
}

void lights::forceReset()
{
	if (!game::isAvailable())
		return;

	for (int i = 0; i < memory::interfaces::hostState->worldbrush.numworldlights; ++i)
	{
		dworldlight_t* light = &memory::interfaces::hostState->worldbrush.worldlights[i];
		const emittype_t type = light->type;

		light->intensity = savedColors.at(i);
	}
}

void lights::shutdown()
{
	forceReset();
}