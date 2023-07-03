#include "weather.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/IBaseClientDll.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/ICollideable.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <utilities/rand.hpp>
#include <SDK/CUtlVector.hpp>

#include <imgui.h>
#include <utilities/console/console.hpp>

#include <mutex>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/levelInitPreEntity.hpp>

namespace
{
	struct WeatherHandler : hooks::FrameStageNotify
	{
		WeatherHandler()
		{
			this->registerRun(weatherController::run);
			this->registerShutdown(weatherController::shutdown);
		}
	} weatherHandler;

	struct WeatherHandlerReset : hooks::LevelInitPreEntity
	{
		WeatherHandlerReset()
		{
			this->registerReset(weatherController::reset);
		}
	} weatherReseter;
}

namespace weatherController
{
	struct WeatherFields_t
	{
		Entity_t* ent;
		Entity_t* networkable;
		bool created;
	} weatherFields;
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/418432-precipitation-effect-similar-llamahook.html
void weatherController::run(FrameStage stage)
{
	if (!game::isAvailable())
		return;

	auto getNetworkable = []() -> Entity_t*
	{
		auto ent = reinterpret_cast<Entity_t*>(memory::interfaces::preciptation->m_createFn(MAX_EDICTS - 1, 0));

		if (!ent)
			return nullptr;

		return ent;
	};

	if (!vars::visuals->world->weather->enabled)
	{
		shutdown();
		return;
	}

	// reduce calling it often
	if (stage == FRAME_RENDER_START && !weatherFields.created)
	{
		weatherFields.networkable = getNetworkable();

		if (!weatherFields.networkable)
			return;

		weatherFields.ent = reinterpret_cast<Entity_t*>((uintptr_t)weatherFields.networkable - 8); // make normal ent from networkbale
		if (!weatherFields.ent)
			return;

		constexpr float halfP = Vec3::MAX_ARG / 2.0f;
		constexpr float halfM = -Vec3::MAX_ARG / 2.0f;

		// 55 8B EC 83 EC 5C 8B C1
		// UNLOCK_PRECIP_TESTING hoooks for more info for other types if you want to try things out
		weatherFields.ent->m_nPrecipType() = PrecipitationType_t::PRECIPITATION_TYPE_SNOW;

		// network streams
		weatherFields.ent->preDataUpdate(DATA_UPDATE_CREATED);
		weatherFields.ent->onPreDataChanged(DATA_UPDATE_CREATED);

		weatherFields.ent->m_vecMins() = Vec3{ halfM, halfM, halfM };
		weatherFields.ent->m_vecMaxs() = Vec3{ halfP, halfP, halfP };

		if (weatherFields.ent->collideable())
		{
			weatherFields.ent->collideable()->OBBMins() = Vec3{ halfM, halfM, halfM };
			weatherFields.ent->collideable()->OBBMaxs() = Vec3{ halfP, halfP, halfP };
		}

		// force this, reason why rain, snow work is due to CClient_Precipitation::OnDataChanged
		weatherFields.ent->onDataChanged(DATA_UPDATE_CREATED);
		weatherFields.ent->postDataUpdate(DATA_UPDATE_CREATED);

		weatherFields.created = true;

		//printf("weather %p\n", weatherFields.ent);
	}
}

struct ConfigWeather
{
	IConVar* convar;
	float defaultVal;
	float min, max;
	float* configRef;
};

void weatherController::implMenu()
{
	const static ConfigWeather cvarLenght
	{
		.convar = memory::interfaces::cvar->findVar("r_rainlength"),
		.defaultVal = memory::interfaces::cvar->findVar("r_rainlength")->getFloat(),
		.min = 0.0f,
		.max = 1.0f,
		.configRef = &vars::visuals->world->weather->length
	};
	const static ConfigWeather cvarRadius
	{
		.convar = memory::interfaces::cvar->findVar("r_rainradius"),
		.defaultVal = memory::interfaces::cvar->findVar("r_rainradius")->getFloat(),
		.min = 0.0f,
		.max = 3000.0f,
		.configRef = &vars::visuals->world->weather->radius
	};
	const static ConfigWeather cvarWidth
	{
		.convar = memory::interfaces::cvar->findVar("r_rainwidth"),
		.defaultVal = memory::interfaces::cvar->findVar("r_rainwidth")->getFloat(),
		.min = 0.0f,
		.max = 5.0f,
		.configRef = &vars::visuals->world->weather->width
	};
	const static ConfigWeather cvarSidevel
	{
		.convar = memory::interfaces::cvar->findVar("r_RainSideVel"),
		.defaultVal = memory::interfaces::cvar->findVar("r_RainSideVel")->getFloat(),
		.min = 50.0f,
		.max = 1000.0f,
		.configRef = &vars::visuals->world->weather->velocity
	};
	const static ConfigWeather cvarAlpha
	{
		.convar = memory::interfaces::cvar->findVar("r_rainalpha"),
		.defaultVal = memory::interfaces::cvar->findVar("r_rainalpha")->getFloat(),
		.min = 0.0f,
		.max = 1.0f,
		.configRef = &vars::visuals->world->weather->alpha
	};
	const static ConfigWeather cvarWindSpeed
	{
		.convar = memory::interfaces::cvar->findVar("cl_windspeed"),
		.defaultVal = memory::interfaces::cvar->findVar("cl_windspeed")->getFloat(),
		.min = 0.0f,
		.max = 1000.0f,
		.configRef = &vars::visuals->world->weather->windSpeed
	};

	static std::array allvars
	{
		cvarLenght,
		cvarRadius,
		cvarWidth,
		cvarSidevel,
		cvarAlpha,
		cvarWindSpeed
	};

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []()
		{
			for (auto& cfg : allvars)
				*cfg.configRef = cfg.defaultVal;
		});

	for(auto& cfg : allvars)
	{
		if(ImGui::SliderFloat(cfg.convar->m_name, cfg.configRef, cfg.min, cfg.max))
			cfg.convar->setValue(*cfg.configRef);

		if (cfg.convar->m_helper)
		{
			ImGui::SameLine();
			ImGui::HelpMarker(cfg.convar->m_helper);
		}
	}

	if (ImGui::Button("Default rain cvars"))
	{
		for (const auto& cfg : allvars)
		{
			*cfg.configRef = cfg.defaultVal;
			cfg.convar->setValue(cfg.defaultVal);
		}
	}
}

void weatherController::reset()
{
	weatherFields.created = false;
}

void weatherController::shutdown()
{
	if (weatherFields.created)
	{
		weatherFields.created = false;
		if (auto w = weatherFields.ent)
		{
			w->release();
			w = nullptr;
			weatherFields.networkable = nullptr;
		}
	}
}