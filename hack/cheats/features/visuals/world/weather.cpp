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

void WeatherController::implMenu()
{
	const static auto cvarLenght = memory::interfaces::cvar->findVar("r_rainlength");
	const static auto cvarRainSpeed = memory::interfaces::cvar->findVar("r_rainspeed");
	const static auto cvarRadius = memory::interfaces::cvar->findVar("r_rainradius");
	const static auto cvarWidth = memory::interfaces::cvar->findVar("r_rainwidth");
	const static auto cvarSidevel = memory::interfaces::cvar->findVar("r_RainSideVel");
	const static auto cvarAlpha = memory::interfaces::cvar->findVar("r_rainalpha");
	const static auto cvarWindSpeed = memory::interfaces::cvar->findVar("cl_windspeed");

	const static std::array defaults =
	{
		std::make_pair(cvarLenght, cvarLenght->getFloat()),
		std::make_pair(cvarRainSpeed, cvarRainSpeed->getFloat()),
		std::make_pair(cvarRadius, cvarRadius->getFloat()),
		std::make_pair(cvarWidth, cvarWidth->getFloat()),
		std::make_pair(cvarSidevel, cvarSidevel->getFloat()),
		std::make_pair(cvarAlpha, cvarAlpha->getFloat()),
		std::make_pair(cvarWindSpeed, cvarWindSpeed->getFloat()),
	};

	if (ImGui::SliderFloat("r_rainlength", &vars::visuals->world->weather->length, 0.0f, 1.0f))
	{
		cvarLenght->setValue(vars::visuals->world->weather->length);
	}
	if (ImGui::SliderFloat("r_rainspeed", &vars::visuals->world->weather->rainSpeed, 100.0f, 1000.0f))
	{
		cvarRainSpeed->setValue(vars::visuals->world->weather->rainSpeed);
	}
	if (ImGui::SliderFloat("r_rainradius", &vars::visuals->world->weather->radius, 0.0f, 3000.0f))
	{
		cvarRadius->setValue(vars::visuals->world->weather->radius);
	}
	if (ImGui::SliderFloat("r_rainwidth", &vars::visuals->world->weather->width, 0.0f, 5.0f))
	{
		cvarWidth->setValue(vars::visuals->world->weather->width);
	}
	if (ImGui::SliderFloat("r_RainSideVel", &vars::visuals->world->weather->velocity, 50.0f, 1000.0f))
	{
		cvarSidevel->setValue(vars::visuals->world->weather->velocity);
	}
	if (ImGui::SliderFloat("r_rainalpha", &vars::visuals->world->weather->alpha, 0.0f, 1.0f))
	{
		cvarAlpha->setValue(vars::visuals->world->weather->alpha);
	}
	if (ImGui::SliderFloat("cl_windspeed", &vars::visuals->world->weather->windSpeed, 0.0f, 1000.0f))
	{
		cvarWindSpeed->setValue(vars::visuals->world->weather->windSpeed);
	}
	if (ImGui::Button("Default rain cvars"))
	{
		for (const auto [convar, defVal] : defaults)
			convar->setValue(defVal);
	}
}

void WeatherController::reset()
{
	m_weather.m_created = false;
}