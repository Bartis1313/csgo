#include "_menu.hpp"

#include "helper.hpp"
#include "precipitation.hpp"
#include "ground.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IEngineSound.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <cheats/game/game.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <config/vars.hpp>

struct ConfigWeather
{
	IConVar* convar;
	float defaultVal;
	float min, max;
	float* configRef;
};

void weather::menu::draw()
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
	const static ConfigWeather cvarParticleDensity
	{
		.convar = memory::interfaces::cvar->findVar("r_RainParticleDensity"),
		.defaultVal = memory::interfaces::cvar->findVar("r_RainParticleDensity")->getFloat(),
		.min = 0.0f,
		.max = 15.0f, // supposed to be max of 1
		.configRef = &vars::visuals->world->weather->particleDensity
	};

	static std::array allvarsNonParticle
	{
		cvarLenght,
			cvarRadius,
			cvarWidth,
			cvarSidevel,
			cvarAlpha,
			cvarWindSpeed
	};

	static std::array allvarsParticle
	{
		cvarParticleDensity
	};

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []()
		{
			for (auto& cfg : allvarsNonParticle)
				*cfg.configRef = cfg.defaultVal;

			for (auto& cfg : allvarsParticle)
				*cfg.configRef = cfg.defaultVal;
		});

	const helper::CustomPrecipitationType_t precipType = helper::configToPrecip(vars::visuals->world->weather->type);
	const bool isParticle = vars::visuals->world->weather->type >= 2;

	if (!isParticle)
	{
		for (auto& cfg : allvarsNonParticle)
		{
			if (ImGui::SliderFloat(cfg.convar->m_name, cfg.configRef, cfg.min, cfg.max))
				cfg.convar->setValue(*cfg.configRef);

			if (cfg.convar->m_helper)
			{
				ImGui::SameLine();
				ImGui::HelpMarker(cfg.convar->m_helper);
			}
		}
	}
	else
	{
		if (ImGui::SliderFloat("Sound volume", &vars::visuals->world->weather->volume, 0.0f, 1.0f))
		{
			memory::interfaces::engineSound->emitAmbientSound(precipitation::pickSoundForPrecip(precipType).data(),
				vars::visuals->world->weather->volume, PITCH_NORM, SND_STOP);

			memory::interfaces::engineSound->emitAmbientSound(precipitation::pickSoundForPrecip(precipType).data(),
				vars::visuals->world->weather->volume, PITCH_NORM, SND_SPAWNING | SND_STOP_LOOPING);
		}

		auto drawGround = []()
		{
			ImGui::Checkbox("Use Ground", &vars::visuals->world->weather->useGround);
			ImGui::SameLine();
			ImGui::HelpMarker("Rain particles types will get very reflective ground\nSnow changes texture to the snow");
			ImGui::SameLine();
			if (ImGui::Button("Create##ground"))
				ground::calledCreate = true;

			ImGui::SameLine();
			if (ImGui::Button("Shutdown##ground"))
				ground::calledShutdown = true;

			ImGui::SameLine();
			ImGui::HelpMarker("It is recommended to not use those, because for now we can't shutdown mats from findmaterial solution\n"
				"Manual solution should reinit textures well, although there are artifacts");
		};

		if (!game::isAvailable())
		{
			drawGround();
		}
		else
		{
			const auto& groundInfo = weather::ground::getGroundInfo();
			auto mapIt = groundInfo.find(memory::interfaces::engine->getLevelName());
			if (mapIt == groundInfo.end())
			{
				ImGui::TextUnformatted("Unsupported map!");
				ImGui::BeginDisabled();
				drawGround();
				ImGui::EndDisabled();
			}
			else
				drawGround();
		}

		for (auto& cfg : allvarsParticle)
		{
			if (ImGui::SliderFloat(cfg.convar->m_name, cfg.configRef, cfg.min, cfg.max))
				cfg.convar->setValue(*cfg.configRef);

			if (cfg.convar->m_helper)
			{
				ImGui::SameLine();
				ImGui::HelpMarker(cfg.convar->m_helper);
			}
		}
	}

	if (!isParticle)
	{
		if (ImGui::Button("Default rain cvars"))
		{
			for (const auto& cfg : allvarsNonParticle)
			{
				*cfg.configRef = cfg.defaultVal;
				cfg.convar->setValue(cfg.defaultVal);
			}
		}
	}
	else
	{
		if (ImGui::Button("Default rain cvars"))
		{
			for (const auto& cfg : allvarsParticle)
			{
				*cfg.configRef = cfg.defaultVal;
				cfg.convar->setValue(cfg.defaultVal);
			}
		}
	}

	if (isParticle)
	{
		/*if (precipType == CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW)
		{
			ImGui::Checkbox("Snow Edit", &vars::visuals->world->weather->snowParticleEdit);
			ImGui::ColorPicker("Inner Near##snow", &vars::visuals->world->weather->snowParticleInnerNearColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Inner Far##snow", &vars::visuals->world->weather->snowParticleInnerFarColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Outer##snow", &vars::visuals->world->weather->snowParticleOuterColor);
		}*/
		if (precipType == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN)
		{
			ImGui::Checkbox("Rain Edit", &vars::visuals->world->weather->rainParticleEdit);
			ImGui::ColorPicker("Inner Near##rain", &vars::visuals->world->weather->rainParticleInnerNearColor);
			/*ImGui::SameLine();
			ImGui::ColorPicker("Inner Far##rain", &vars::visuals->world->weather->rainParticleInnerFarColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Outer##rain", &vars::visuals->world->weather->rainParticleOuterColor);*/
		}
		else if (precipType == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM)
		{
			ImGui::Checkbox("Rain Storm Edit", &vars::visuals->world->weather->rainStormParticleEdit);
			ImGui::ColorPicker("Inner Near##rainst", &vars::visuals->world->weather->rainStormParticleInnerNearColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Inner Far##rainst", &vars::visuals->world->weather->rainStormParticleInnerFarColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Outer##rainst", &vars::visuals->world->weather->rainStormParticleOuterColor);
		}

		ImGui::SliderFloat("Near##w", &vars::visuals->world->weather->customNear, 0.0f, 200.0f);
		ImGui::SliderFloat("Far##w", &vars::visuals->world->weather->customFar, 0.0f, 500.0f);
	}
}