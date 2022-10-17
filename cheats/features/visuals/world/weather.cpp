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
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <utilities/rand.hpp>
#include <dependencies/ImGui/imgui.h>

void WeatherController::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

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
			}
		}
	};

	auto getNetworkable = [this]() -> Entity_t*
	{
		for (auto _class = interfaces::client->getAllClasses(); _class && !m_weather.m_preciptation; _class = _class->m_next)
		{
			if (_class->m_classID == CPrecipitation)
				m_weather.m_preciptation = _class;
		}

		auto ent = reinterpret_cast<Entity_t*>(m_weather.m_preciptation->m_createFn(MAX_EDICTS - 1, Random::getRandom<int>(0x0, 0xFFF)));
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
	if (!m_weather.m_created)
	{
		if (!getNetworkable())
			return;

		m_weather.m_ent = reinterpret_cast<Entity_t*>(interfaces::entList->getClientEntity(MAX_EDICTS - 1));
		if (!m_weather.m_ent)
			return;

		constexpr float halfP = Vec3::MAX_ARG / 2.0f;
		constexpr float halfM = -Vec3::MAX_ARG / 2.0f;

		m_weather.m_ent->m_nPrecipType() = PRECIPITATION_TYPE_SNOW;

		m_weather.m_ent->preDataUpdate(DATA_UPDATE_CREATED);
		m_weather.m_ent->onPreDataChanged(DATA_UPDATE_CREATED);

		m_weather.m_ent->collideable()->OBBMins() = Vec3{ halfM, halfM, halfM };
		m_weather.m_ent->collideable()->OBBMaxs() = Vec3{ halfP, halfP, halfP };

		m_weather.m_ent->onDataChanged(DATA_UPDATE_CREATED);
		m_weather.m_ent->postDataUpdate(DATA_UPDATE_CREATED);

		m_weather.m_created = true;
	}
}

void WeatherController::implMenu()
{
	const static auto cvarLenght = interfaces::cvar->findVar(XOR("r_rainlength"));
	const static auto cvarRainSpeed = interfaces::cvar->findVar(XOR("r_rainspeed"));
	const static auto cvarRadius = interfaces::cvar->findVar(XOR("r_rainradius"));
	const static auto cvarWidth = interfaces::cvar->findVar(XOR("r_rainwidth"));
	const static auto cvarSidevel = interfaces::cvar->findVar(XOR("r_RainSideVel"));
	const static auto cvarAlpha = interfaces::cvar->findVar(XOR("r_rainalpha"));
	const static auto cvarWindSpeed = interfaces::cvar->findVar(XOR("cl_windspeed"));

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

	if (ImGui::SliderFloat(XOR("r_rainlength"), &vars::visuals->world->weather->length, 0.0f, 1.0f))
	{
		cvarLenght->setValue(vars::visuals->world->weather->length);
	}
	if (ImGui::SliderFloat(XOR("r_rainspeed"), &vars::visuals->world->weather->rainSpeed, 0.0f, 1000.0f))
	{
		cvarRainSpeed->setValue(vars::visuals->world->weather->rainSpeed);
	}
	if (ImGui::SliderFloat(XOR("r_rainradius"), &vars::visuals->world->weather->radius, 0.0f, 3000.0f))
	{
		cvarRadius->setValue(vars::visuals->world->weather->radius);
	}
	if (ImGui::SliderFloat(XOR("r_rainwidth"), &vars::visuals->world->weather->width, 0.0f, 5.0f))
	{
		cvarWidth->setValue(vars::visuals->world->weather->width);
	}
	if (ImGui::SliderFloat(XOR("r_RainSideVel"), &vars::visuals->world->weather->velocity, 0.0f, 1000.0f))
	{
		cvarSidevel->setValue(vars::visuals->world->weather->velocity);
	}
	if (ImGui::SliderFloat(XOR("r_rainalpha"), &vars::visuals->world->weather->alpha, 0.0f, 1.0f))
	{
		cvarAlpha->setValue(vars::visuals->world->weather->alpha);
	}
	if (ImGui::SliderFloat(XOR("cl_windspeed"), &vars::visuals->world->weather->windSpeed, 0.0f, 1000.0f))
	{
		cvarWindSpeed->setValue(vars::visuals->world->weather->windSpeed);
	}
	if (ImGui::Button(XOR("Default rain cvars")))
	{
		for (const auto [convar, defVal] : defaults)
			convar->setValue(defVal);
	}
}

void WeatherReset::run(MapStruct* map)
{
	g_WeatherController->m_weather.m_created = false;
}