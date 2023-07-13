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
#include <SDK/IMaterialSystem.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <utilities/rand.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/IEngineSound.hpp>
#include <SDK/IPhysicsCollision.hpp>

#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <utilities/console/console.hpp>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/levelInitPreEntity.hpp>
#include <cheats/hooks/clientModeCSNormalEvent.hpp>

namespace
{
	struct WeatherHandler : hooks::FrameStageNotify
	{
		WeatherHandler()
		{
			this->registerInit(weatherController::init);
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

	struct WeatherSound : hooks::ClientModeCSNormalEvent
	{
		WeatherSound()
		{
			this->registerEvent(weatherController::handleSounds);
		}
	} weatherSounds;
}

namespace weatherController
{
	// thanks clarification, for more reference see dump logs on RAIN maps
	constexpr std::array<uint8_t, 546> weatherBuffer
	{
		0xB8, 0x01, 0x00, 0x00, 0x56, 0x50, 0x48, 0x59, 0x00, 0x01, 0x00, 0x00, 0x9C, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
		0x20, 0x16, 0x6A, 0xC1, 0xC0, 0x0E, 0x1C, 0xC1, 0x80, 0x13, 0xD0, 0x3F, 0xE2, 0x26, 0x11, 0x48,
		0xE2, 0x26, 0x11, 0x48, 0xE2, 0x26, 0x11, 0x48, 0x72, 0x4E, 0x08, 0x44, 0xD1, 0x9C, 0x01, 0x00,
		0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x56, 0x50, 0x53,
		0xD0, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
		0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x12, 0x00,
		0x01, 0x80, 0x00, 0x00, 0x02, 0x00, 0xFD, 0x7F, 0x01, 0x00, 0x15, 0x00, 0x03, 0x00, 0x28, 0x00,
		0x02, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00, 0x0F, 0x00, 0x01, 0x00, 0xF6, 0x7F,
		0x03, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x05, 0x00, 0x15, 0x00, 0x04, 0x00, 0xFA, 0x7F,
		0x04, 0x70, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x13, 0x00, 0x05, 0x00, 0xFA, 0x7F,
		0x05, 0x60, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x7F, 0x02, 0x00, 0x18, 0x00, 0x06, 0x00, 0xFA, 0x7F,
		0x06, 0x50, 0x00, 0x00, 0x01, 0x00, 0xF1, 0x7F, 0x04, 0x00, 0x03, 0x00, 0x03, 0x00, 0xEB, 0x7F,
		0x07, 0x40, 0x00, 0x00, 0x03, 0x00, 0xFD, 0x7F, 0x04, 0x00, 0x04, 0x00, 0x07, 0x00, 0x0C, 0x00,
		0x08, 0x10, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0xFC, 0x7F, 0x04, 0x00, 0xEB, 0x7F,
		0x09, 0x00, 0x00, 0x00, 0x05, 0x00, 0xED, 0x7F, 0x06, 0x00, 0x03, 0x00, 0x07, 0x00, 0xFA, 0x7F,
		0x0A, 0x30, 0x00, 0x00, 0x07, 0x00, 0xFD, 0x7F, 0x06, 0x00, 0x03, 0x00, 0x03, 0x00, 0xF4, 0x7F,
		0x0B, 0x20, 0x00, 0x00, 0x03, 0x00, 0xFD, 0x7F, 0x06, 0x00, 0xE8, 0x7F, 0x02, 0x00, 0xD8, 0x7F,
		0x46, 0xAD, 0x9D, 0xC3, 0x1F, 0x0D, 0x9C, 0xC3, 0x80, 0xAE, 0xAA, 0x43, 0x00, 0x00, 0x00, 0x00,
		0x46, 0xAD, 0x9D, 0xC3, 0x33, 0x4C, 0x92, 0x43, 0x80, 0xAE, 0xAA, 0x43, 0x00, 0x00, 0x00, 0x00,
		0x46, 0xAD, 0x9D, 0xC3, 0x1F, 0x0D, 0x9C, 0xC3, 0x59, 0x0E, 0xA9, 0xC3, 0x00, 0x00, 0x00, 0x00,
		0x46, 0xAD, 0x9D, 0xC3, 0x33, 0x4C, 0x92, 0x43, 0x59, 0x0E, 0xA9, 0xC3, 0x00, 0x00, 0x00, 0x00,
		0xE4, 0x0B, 0x8F, 0x43, 0x33, 0x4C, 0x92, 0x43, 0x80, 0xAE, 0xAA, 0x43, 0x00, 0x00, 0x00, 0x00,
		0xE4, 0x0B, 0x8F, 0x43, 0x1F, 0x0D, 0x9C, 0xC3, 0x80, 0xAE, 0xAA, 0x43, 0x00, 0x00, 0x00, 0x00,
		0xE4, 0x0B, 0x8F, 0x43, 0x1F, 0x0D, 0x9C, 0xC3, 0x59, 0x0E, 0xA9, 0xC3, 0x00, 0x00, 0x00, 0x00,
		0xE4, 0x0B, 0x8F, 0x43, 0x33, 0x4C, 0x92, 0x43, 0x59, 0x0E, 0xA9, 0xC3, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xB0, 0xFE, 0xFF, 0xFF, 0x20, 0x16, 0x6A, 0xC1, 0xC0, 0x0E, 0x1C, 0xC1,
		0x80, 0x13, 0xD0, 0x3F, 0x72, 0x4E, 0x08, 0x44, 0x8A, 0x8B, 0x9C, 0x00, 0x73, 0x6F, 0x6C, 0x69,
		0x64, 0x20, 0x7B, 0x0A, 0x22, 0x69, 0x6E, 0x64, 0x65, 0x78, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A,
		0x22, 0x6D, 0x61, 0x73, 0x73, 0x22, 0x20, 0x22, 0x35, 0x30, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30,
		0x30, 0x30, 0x30, 0x30, 0x22, 0x0A, 0x22, 0x73, 0x75, 0x72, 0x66, 0x61, 0x63, 0x65, 0x70, 0x72,
		0x6F, 0x70, 0x22, 0x20, 0x22, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6C, 0x74, 0x22, 0x0A, 0x22, 0x76,
		0x6F, 0x6C, 0x75, 0x6D, 0x65, 0x22, 0x20, 0x22, 0x31, 0x35, 0x30, 0x38, 0x30, 0x32, 0x33, 0x32,
		0x30, 0x35, 0x38, 0x38, 0x38, 0x30, 0x2E, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x22, 0x0A, 0x7D,
		0x0A, 0x00
	};

	enum class CustomPrecipitationType_t
	{
		PRECIPITATION_TYPE_NONE = -1,
		PRECIPITATION_TYPE_RAIN = 0,
		PRECIPITATION_TYPE_SNOW,
		PRECIPITATION_TYPE_PARTICLERAIN = 4,
		PRECIPITATION_TYPE_PARTICLERAINSTORM = 6,
		PRECIPITATION_TYPE_PARTICLESNOW,
		PRECIPITATION_TYPE_CUSTOM,
	};

	struct WeatherFields_t
	{
		Precipitation_t* ent;
		Entity_t* networkable;
		vcollide_t collide;
	} weatherFields;

	bool done = false;
	bool newMap = false;

	void create(PrecipitationType_t precip);
	void loadCollide();
	void createCustom();
	CustomPrecipitationType_t configToPrecip(int cfg);
	std::string_view pickSoundForPrecip(CustomPrecipitationType_t precip);
}

void weatherController::init()
{
	
}

void weatherController::create(PrecipitationType_t precip)
{
	auto getNetworkable = []() -> Entity_t*
	{
		auto ent = reinterpret_cast<Entity_t*>(memory::interfaces::preciptation->m_createFn(MAX_EDICTS - 1, 0));

		if (!ent)
			return nullptr;

		return ent;
	};

	const auto net = getNetworkable();
	weatherFields.networkable = net;
	if (!net)
		return;

	loadCollide();

	weatherFields.ent = reinterpret_cast<Precipitation_t*>((uintptr_t)weatherFields.networkable - 8); // make normal ent from networkbale
	if (!weatherFields.ent)
		return;

	weatherFields.ent->m_nPrecipType() = precip;

	weatherFields.ent->m_nModelIndex() = uniqueModelIndex;
	weatherFields.ent->m_bDormant() = false;

	const bool isNonParticle = precip == PRECIPITATION_TYPE_RAIN || precip == PRECIPITATION_TYPE_SNOW;
	if (isNonParticle)
	{
		if (const auto w = memory::interfaces::gameWorld())
		{
			const auto& mins = w->m_WorldMins();
			const auto& maxs = w->m_WorldMaxs();

			weatherFields.ent->collideable()->OBBMins() = mins;
			weatherFields.ent->collideable()->OBBMaxs() = maxs;
		}
	}

	weatherFields.ent->onDataChanged(DATA_UPDATE_CREATED);

	done = true;
}

void weatherController::createCustom()
{
	// play with fields I provided, change particles to some good from:
	// https://developer.valvesoftware.com/wiki/List_of_CS:GO_Particles
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/418432-precipitation-effect-similar-llamahook.html
void weatherController::run(FrameStage stage)
{
	if (stage != FRAME_START)
		return;

	if (!game::isAvailable())
		return;

	static int lastType = vars::visuals->world->weather->type;
	int lastCopyPostChanged = lastType;
	bool shouldCreate = false;
	if (lastType != vars::visuals->world->weather->type)
	{
		shouldCreate = true;
		lastType = vars::visuals->world->weather->type;
	}

	const CustomPrecipitationType_t precipType = configToPrecip(lastType);
	const CustomPrecipitationType_t prePrecipType = configToPrecip(lastCopyPostChanged);

	static std::once_flag onceFlag;
	std::call_once(onceFlag, [&shouldCreate, precipType]()
		{
			if (!shouldCreate && precipType != CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
				shouldCreate = true;
		});

	if (precipType == CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
	{
		if (shouldCreate)
		{
			// stop sounds used by last weather
			memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(prePrecipType).data(),
				vars::visuals->world->weather->volume, PITCH_NORM, SND_STOP);

			game::localPlayer->stopSound(pickSoundForPrecip(prePrecipType).data());
			shutdown();
		}

		return;
	}

	if (precipType != CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
	{
		if (newMap)
		{
			shouldCreate = true;
			newMap = false;
		}
	}

	if (shouldCreate)
	{
		shutdown();
		memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(prePrecipType).data(),
			vars::visuals->world->weather->volume, PITCH_NORM, SND_STOP);
		create(static_cast<PrecipitationType_t>(precipType));

		// creating the initial one
		memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
			vars::visuals->world->weather->volume, PITCH_NORM, SND_SPAWNING | SND_STOP_LOOPING);
	}

	if (precipType == CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW)
	{
		// just loop thru manually, this sound for some reason doesnt loop
		static float lastTime = memory::interfaces::globalVars->m_curtime;
		if (memory::interfaces::globalVars->m_curtime - lastTime >= 6.0f)
		{
			memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
				vars::visuals->world->weather->volume, PITCH_NORM, SND_SPAWNING | SND_STOP_LOOPING);

			lastTime = memory::interfaces::globalVars->m_curtime;
		}
	}
}

void weatherController::handleSounds(IGameEvent* _event)
{
	if (std::string_view{ _event->getName() } != "round_start")
		return;

	const CustomPrecipitationType_t precipType = configToPrecip(vars::visuals->world->weather->type);
	if (precipType == CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
		return;

	if (!done) // highly unlikely
		return;

	memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
		vars::visuals->world->weather->volume, PITCH_NORM, SND_SPAWNING | SND_STOP_LOOPING);
}

void weatherController::reset()
{
	// engine does it
	//shutdown();
	weatherFields.ent = nullptr;

	newMap = true;
}

void weatherController::shutdown()
{
	if (weatherFields.ent)
	{
		// if executed automatically, then remove this entry sound at all
		if (globals::isShutdown)
		{
			const CustomPrecipitationType_t precipType = configToPrecip(vars::visuals->world->weather->type);
			if (precipType != CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
			{
				memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
					vars::visuals->world->weather->volume, PITCH_NORM, SND_STOP);
			}
		}

		//memory::gPrecipitations().FindAndRemove(weatherFields.ent);		 
		memory::precipDestruct()(weatherFields.ent);
		weatherFields.ent = nullptr;
		memory::interfaces::physicsCollision->vcollideUnload(&weatherFields.collide);
		done = false;
	}
}

void weatherController::loadCollide()
{
	// weather solid counts is always 1, buffer is about 530-550 size with always very similar data, see dumps
	memory::interfaces::physicsCollision->vcollideLoad(&weatherFields.collide, 1, reinterpret_cast<const char*>(weatherBuffer.data()), weatherBuffer.size(), false);
}

vcollide_t& weatherController::getCollide()
{
	return weatherFields.collide;
}

weatherController::CustomPrecipitationType_t weatherController::configToPrecip(int cfg)
{
	switch (cfg)
	{
	case 1:
		return CustomPrecipitationType_t::PRECIPITATION_TYPE_RAIN;
	case 2:
		return CustomPrecipitationType_t::PRECIPITATION_TYPE_SNOW;
	case 3:
		return CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN;
	case 4:
		return CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM;
	case 5:
		return CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW;
	default:
		return CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE;
	}
}

std::string_view weatherController::pickSoundForPrecip(CustomPrecipitationType_t precip)
{
	// some sounds may not play in a loop

	switch (precip)
	{
	case CustomPrecipitationType_t::PRECIPITATION_TYPE_RAIN:
		return "ambient/weather/aztec_rain_lp_01.wav";
	case CustomPrecipitationType_t::PRECIPITATION_TYPE_SNOW:
		return "ambient/wind/lightwind_02.wav";
	case CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN:
		return "ambient/weather/rumble_rain_nowind.wav";
	case CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM:
		return "ambient/weather/rumble_rain.wav";
	case CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW:
		return "ambient/wind/windgust.wav";
	default:
		return "";
	}
}

void weatherController::handleGroundMaterials()
{
	// handle ground
	// ideas are:
	// 1 cubemap as "raytrace" for the wet effect
	// 2 replace manually ground texture on all maps lol (for snow)
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
	const static ConfigWeather cvarParticleDensity
	{
		.convar = memory::interfaces::cvar->findVar("r_RainParticleDensity"),
		.defaultVal = memory::interfaces::cvar->findVar("r_RainParticleDensity")->getFloat(),
		.min = 0.0f,
		.max = 10.0f, // supposed to be max of 1
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

	const CustomPrecipitationType_t precipType = configToPrecip(vars::visuals->world->weather->type);
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
			memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
				vars::visuals->world->weather->volume, PITCH_NORM, SND_STOP);

			memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
				vars::visuals->world->weather->volume, PITCH_NORM, SND_SPAWNING | SND_STOP_LOOPING);
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
		if (precipType == CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN)
		{
			ImGui::Checkbox("Rain Edit", &vars::visuals->world->weather->rainParticleEdit);
			ImGui::ColorPicker("Inner Near##rain", &vars::visuals->world->weather->rainParticleInnerNearColor);
			/*ImGui::SameLine();
			ImGui::ColorPicker("Inner Far##rain", &vars::visuals->world->weather->rainParticleInnerFarColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Outer##rain", &vars::visuals->world->weather->rainParticleOuterColor);*/
		}
		else if (precipType == CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM)
		{
			ImGui::Checkbox("Rain Storm Edit", &vars::visuals->world->weather->rainStormParticleEdit);
			ImGui::ColorPicker("Inner Near##rainst", &vars::visuals->world->weather->rainStormParticleInnerNearColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Inner Far##rainst", &vars::visuals->world->weather->rainStormParticleInnerFarColor);
			ImGui::SameLine();
			ImGui::ColorPicker("Outer##rainst", &vars::visuals->world->weather->rainStormParticleOuterColor);
		}
	}
}