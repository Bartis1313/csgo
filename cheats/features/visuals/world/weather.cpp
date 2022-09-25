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

void WeatherController::init()
{

}

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
	if (globals::isShutdown || !config.get<bool>(vars.bWeather))
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

		constexpr float halfP = Vector::MAX_ARG / 2.0f;
		constexpr float halfM = -Vector::MAX_ARG / 2.0f;

		m_weather.m_ent->m_nPrecipType() = PRECIPITATION_TYPE_SNOW;

		m_weather.m_ent->preDataUpdate(DATA_UPDATE_CREATED);
		m_weather.m_ent->onPreDataChanged(DATA_UPDATE_CREATED);

		m_weather.m_ent->collideable()->OBBMins() = { halfM, halfM, halfM };
		m_weather.m_ent->collideable()->OBBMaxs() = { halfP, halfP, halfP };

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

	if (ImGui::SliderFloat(XOR("r_rainlength"), &config.getRef<float>(vars.fWeatherRainLenght), 0.0f, 1.0f))
	{
		cvarLenght->setValue(config.get<float>(vars.fWeatherRainLenght));
	}
	if (ImGui::SliderFloat(XOR("r_rainspeed"), &config.getRef<float>(vars.fWeatherRainSpeed), 0.0f, 1000.0f))
	{
		cvarRainSpeed->setValue(config.get<float>(vars.fWeatherRainSpeed));
	}
	if (ImGui::SliderFloat(XOR("r_rainradius"), &config.getRef<float>(vars.fWeatherRainRadius), 0.0f, 3000.0f))
	{
		cvarRadius->setValue(config.get<float>(vars.fWeatherRainRadius));
	}
	if (ImGui::SliderFloat(XOR("r_rainwidth"), &config.getRef<float>(vars.fWeatherRainWidth), 0.0f, 5.0f))
	{
		cvarWidth->setValue(config.get<float>(vars.fWeatherRainWidth));
	}
	if (ImGui::SliderFloat(XOR("r_RainSideVel"), &config.getRef<float>(vars.fWeatherRainSideVel), 0.0f, 1000.0f))
	{
		cvarSidevel->setValue(config.get<float>(vars.fWeatherRainSideVel));
	}
	if (ImGui::SliderFloat(XOR("r_rainalpha"), &config.getRef<float>(vars.fWeatherRainAlpha), 0.0f, 1.0f))
	{
		cvarAlpha->setValue(config.get<float>(vars.fWeatherRainAlpha));
	}
	if (ImGui::SliderFloat(XOR("cl_windspeed"), &config.getRef<float>(vars.fWeatherWindSpeed), 0.0f, 1000.0f))
	{
		cvarWindSpeed->setValue(config.get<float>(vars.fWeatherWindSpeed));
	}
	if (ImGui::Button(XOR("Default rain cvars")))
	{
		for (const auto [convar, defVal] : defaults)
			convar->setValue(defVal);
	}
}

void WeatherReset::init()
{

}

void WeatherReset::run(MapStruct* map)
{
	g_WeatherController.m_weather.m_created = false;
}