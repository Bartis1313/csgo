#include "weather.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/IBaseClientDll.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/ICollideable.hpp>
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

#include <imgui.h>
#include <utilities/console/console.hpp>

#include <mutex>

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/418432-precipitation-effect-similar-llamahook.html
void WeatherController::run(int frame)
{
	if (!game::isAvailable())
		return;

	// when enable/shutdown
	auto reset = [this]()
	{
		if (m_weather.m_created)
		{
			m_weather.m_created = false;
			if (auto w = m_weather.m_ent)
			{
				w->release();
				w = nullptr;
				m_weather.m_networkable = nullptr;
			}
		}
	};

	auto getNetworkable = [this]() -> Entity_t*
	{
		auto ent = reinterpret_cast<Entity_t*>(memory::interfaces::preciptation->m_createFn(MAX_EDICTS - 1, 0));

		if (!ent)
			return nullptr;

		return ent;
	};

	// it will only actually reset when the rain is created
	if (globals::isShutdown || !vars::visuals->world->weather->enabled)
	{
		reset();
		return;
	}

	// reduce calling it often
	if (frame == FRAME_RENDER_START && !m_weather.m_created)
	{
		m_weather.m_networkable = getNetworkable();

		if (!m_weather.m_networkable)
			return;

		m_weather.m_ent = reinterpret_cast<Entity_t*>((uintptr_t)m_weather.m_networkable - 8); // make normal ent from networkbale
		if (!m_weather.m_ent)
			return;

		constexpr float halfP = Vec3::MAX_ARG / 2.0f;
		constexpr float halfM = -Vec3::MAX_ARG / 2.0f;

		m_weather.m_ent->m_nPrecipType() = PrecipitationType_t::PRECIPITATION_TYPE_SNOW;

		// network streams
		m_weather.m_ent->preDataUpdate(DATA_UPDATE_CREATED);
		m_weather.m_ent->onPreDataChanged(DATA_UPDATE_CREATED);

		m_weather.m_ent->collideable()->OBBMins() = Vec3{ halfM, halfM, halfM };
		m_weather.m_ent->collideable()->OBBMaxs() = Vec3{ halfP, halfP, halfP };

		// force this, reason why rain, snow work is due to CClient_Precipitation::OnDataChanged
		m_weather.m_ent->onDataChanged(DATA_UPDATE_CREATED);
		m_weather.m_ent->postDataUpdate(DATA_UPDATE_CREATED);

		m_weather.m_created = true;

		// haven't tried those! just a quick lookup

		// FF 35 ? ? ? ? 89 75 FC E8 ? ? ? ? 5F
		// looks like g_Precipitations - and looks like there is more code for this
		// E8 ? ? ? ? 8B 0D ? ? ? ? 85 C9 74 10 8B 01 6A 01 
		// ^ draw global func
		// 55 8B EC 51 53 56 57 8B F1 E8 ? ? ? ? 
		// ^ constructor

		memory::precipitationInit()(m_weather.m_ent); // force creation

		//printf("weather %p\n", m_weather.m_ent);
	}

	if (frame == FRAME_START && m_weather.m_created)
	{
		memory::precipitationClientThink()((void*)((uintptr_t)m_weather.m_ent + 12)); // make to thinkable
	}
}

struct ConfigWeather
{
	IConVar* convar;
	float defaultVal;
	float min, max;
	float* configRef;
};

void WeatherController::implMenu()
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

void WeatherController::reset()
{
	m_weather.m_created = false;
}