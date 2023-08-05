#include "precipitation.hpp"

#include "helper.hpp"

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
#include <SDK/IVEngineClient.hpp>
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
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/IPhysicsCollision.hpp>
#include <SDK/IVEffects.hpp>

#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <utilities/console/console.hpp>
#include <utilities/math/math.hpp>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/levelInitPreEntity.hpp>
#include <cheats/hooks/clientModeCSNormalEvent.hpp>
#include <cheats/hooks/drawModelExecute.hpp>
#include <cheats/features/visuals/chams/factory/factory.hpp>

namespace
{
	struct WeatherHandler : hooks::FrameStageNotify
	{
		WeatherHandler()
		{
			this->registerInit(weather::precipitation::init);
			this->registerRun(weather::precipitation::run);
			this->registerShutdown(weather::precipitation::shutdown);
		}
	} weatherHandler;

	struct WeatherHandlerReset : hooks::LevelInitPreEntity
	{
		WeatherHandlerReset()
		{
			this->registerReset(weather::precipitation::reset);
		}
	} weatherReseter;

	struct WeatherFiles : hooks::DrawModelExecute
	{
		WeatherFiles()
		{
			this->registerInit(weather::precipitation::preacheCustomFiles);
		}
	} weatherFiles;
}

namespace weather::precipitation
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

	struct WeatherFields_t
	{
		Precipitation_t* ent;
		Entity_t* networkable;
		vcollide_t collide;
	} weatherFields;

	bool done = false;
	bool newMap = false;
	float lastTimeStorm = 0.0f;
	size_t lightRapidsCounter = 0;
	DLight_t* lightStrike = nullptr;
	
	std::vector<std::pair<IMaterial*, std::string_view>> groundWorld;

	void create(PrecipitationType_t precip);
	void loadCollide();
	void createCustom();
	std::string_view pickRandomSoundForThunder();
}

#include <resources/resource.h>

HGLOBAL loadResourceData(HMODULE hModule, const int idRes, const char* resourceType, DWORD& dataSize) 
{
	HRSRC hResource = FindResourceA(hModule, MAKEINTRESOURCEA(idRes), resourceType);
	if (!hResource)
		return nullptr;

	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	if (!hGlobal)
		return nullptr;

	dataSize = SizeofResource(hModule, hResource);
	return hGlobal;
}

std::vector<char> loadFile(HMODULE hModule, const int idRes, const char* resourceType)
{
	DWORD dataSize;
	HGLOBAL hGlobal = loadResourceData(hModule, idRes, resourceType, dataSize);
	if (!hGlobal)
		return {};

	const char* dataPtr = static_cast<const char*>(LockResource(hGlobal));
	if (!dataPtr)
		return {};

	UnlockResource(hGlobal);

	return std::vector<char>(dataPtr, dataPtr + dataSize);
}

void saveResourceToDest(const std::vector<char>& data, const std::filesystem::path& path)
{
	if (data.empty())
	{
		std::cout << "Empty for " << path << "\n";
		return;
	}

	std::ofstream resFile{ path, std::ios::binary };
	resFile.write(data.data(), data.size());
}

#include <SDK/IBaseFileSystem.hpp>
#include <SDK/INetworkStringTableContainer.hpp>
#include <SDK/interfaces/interfaces.hpp>

// something broken, they never show :(
void weather::precipitation::init()
{
	const auto pathSprites = std::filesystem::current_path() / "csgo" / "materials" / "sprites";

	const auto bluelightningVMT = loadFile(globals::instance, IDR_VMT_FILE2, "VMT_FILE");
	saveResourceToDest(bluelightningVMT, pathSprites / "bluelightning.vmt");
	
	const auto bluelightningVTF = loadFile(globals::instance, IDR_VTF_FILE2, "VTF_FILE");
	saveResourceToDest(bluelightningVTF, pathSprites / "bluelightning.vtf");

	const auto bluelight1VMT = loadFile(globals::instance, IDR_VMT_FILE3, "VMT_FILE");
	saveResourceToDest(bluelight1VMT, pathSprites / "bluelight1.vmt");

	const auto bluelight1VTF = loadFile(globals::instance, IDR_VTF_FILE3, "VTF_FILE");
	saveResourceToDest(bluelight1VTF, pathSprites / "bluelight1.vtf");

	const auto lgtningVMT = loadFile(globals::instance, IDR_VMT_FILE4, "VMT_FILE");
	saveResourceToDest(lgtningVMT, pathSprites / "lgtning.vmt");

	const auto lgtningVTF = loadFile(globals::instance, IDR_VTF_FILE4, "VTF_FILE");
	saveResourceToDest(lgtningVTF, pathSprites / "lgtning.vtf");

	const auto physcannon_bluelight1VMT = loadFile(globals::instance, IDR_VMT_FILE5, "VMT_FILE");
	saveResourceToDest(physcannon_bluelight1VMT, pathSprites / "physcannon_bluelight1.vmt");

	const auto physcannon_bluelight1VTF = loadFile(globals::instance, IDR_VTF_FILE5, "VTF_FILE");
	saveResourceToDest(physcannon_bluelight1VTF, pathSprites / "physcannon_bluelight1.vtf");

	const auto physcannon_bluelight1bVMT = loadFile(globals::instance, IDR_VMT_FILE6, "VMT_FILE");
	saveResourceToDest(physcannon_bluelight1bVMT, pathSprites / "physcannon_bluelight1b.vmt");

	const auto physcannon_bluelight1bVTF = loadFile(globals::instance, IDR_VTF_FILE6, "VTF_FILE");
	saveResourceToDest(physcannon_bluelight1bVTF, pathSprites / "physcannon_bluelight1b.vtf");

	const auto pathParticles = std::filesystem::current_path() / "csgo" / "materials" / "particle";

	const auto snowflakeVMT = loadFile(globals::instance, IDR_VMT_FILE1, "VMT_FILE");
	saveResourceToDest(snowflakeVMT, pathParticles / "snowflake.vmt");

	const auto snowflakeVTF = loadFile(globals::instance, IDR_VTF_FILE1, "VTF_FILE");
	saveResourceToDest(snowflakeVTF, pathParticles / "snowflake.vtf");
}

void weather::precipitation::preacheCustomFiles()
{
	precacheModel("materials/sprites/bluelightning.vmt");
	precacheModel("materials/sprites/bluelight1.vmt");
	precacheModel("materials/sprites/lgtning.vmt");
	precacheModel("materials/sprites/physcannon_bluelight1.vmt");
	precacheModel("materials/sprites/physcannon_bluelight1b.vmt");
	precacheModel("materials/particle/snowflake.vmt");
}

void weather::precipitation::create(PrecipitationType_t precip)
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

	weatherFields.ent->getClientAlphaProperty()->setAlphaModulation(255);
	
	assert(memory::interfaces::gameWorld());

	const auto& mins = memory::interfaces::gameWorld->m_WorldMins();
	const auto& maxs = memory::interfaces::gameWorld->m_WorldMaxs();

	weatherFields.ent->getCollideable()->OBBMins() = mins;
	weatherFields.ent->getCollideable()->OBBMaxs() = maxs;

	weatherFields.ent->onDataChanged(DATA_UPDATE_CREATED);

	done = true;
}

void weather::precipitation::createCustom()
{
	// play with fields I provided, change particles to some good from:
	// https://developer.valvesoftware.com/wiki/List_of_CS:GO_Particles
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/418432-precipitation-effect-similar-llamahook.html
void weather::precipitation::run(FrameStage stage)
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

	const helper::CustomPrecipitationType_t precipType = helper::configToPrecip(lastType);
	const helper::CustomPrecipitationType_t prePrecipType = helper::configToPrecip(lastCopyPostChanged);

	static std::once_flag onceFlag;
	std::call_once(onceFlag, [&shouldCreate, precipType]()
		{
			if (!shouldCreate && precipType != helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
				shouldCreate = true;
		});

	if (precipType == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
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

	if (precipType != helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
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

	if (precipType == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW)
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

	if (!vars::visuals->world->weather->storm)
	{
		lightRapidsCounter = 0;
		if (lightStrike)
		{
			lightStrike->m_die = memory::interfaces::globalVars->m_curtime;
			lightStrike = nullptr;
		}

		return;
	}

	constexpr size_t maxRapids = 3;
	constexpr float radius = 1200.0f;
	if (precipType == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM)
	{
		if (memory::interfaces::globalVars->m_curtime > lastTimeStorm)
		{
			const Vec3 directionVector = math::angleVec(game::localPlayer->absAngles());
			const Vec3 localHead = game::localPlayer->getHitboxPos(HITBOX_HEAD);
			const Vec3 newPosition
			{
				localHead[0] + (directionVector[0] * 50.0f),
				localHead[1] + (directionVector[1] * 50.0f),
				localHead[2] + 200.0f // dont care, just add a lot
			};

			if (vars::visuals->world->weather->saveFpsStorm)
			{
				// do the fullbright, white ambient + forced tone
			}
			else
			{
				// this is far from proper, Clight seems better
				lightStrike = memory::interfaces::efx->clAllocDLight(1337);
				lightStrike->m_key = 1337;
				//dynamic->m_style = 10; // DUMPED
				lightStrike->m_radius = radius;
				//dynamic->m_flags = 2563; // DUMPED
				float m_OuterAngle = 0.0f;
				float m_InnerAngle = 0.0f;
				if (m_OuterAngle > 0.0f)
					lightStrike->m_flags |= DLIGHT_NO_WORLD_ILLUMINATION;
				lightStrike->m_color = Colors::White;
				lightStrike->m_exponent = 7;
				lightStrike->m_origin = newPosition;
				lightStrike->m_innerAngle = m_InnerAngle;
				lightStrike->m_outerAngle = m_OuterAngle;
				lightStrike->m_die = memory::interfaces::globalVars->m_curtime + 1e6f;
				lightStrike->m_direction = directionVector;

				memory::interfaces::engineSound->emitAmbientSound(pickRandomSoundForThunder().data(),
					vars::visuals->world->weather->volume, PITCH_NORM, SND_SPAWNING | SND_STOP_LOOPING);
			}

			lastTimeStorm = memory::interfaces::globalVars->m_curtime + Random::getRandom<float>(2.0f, 5.0f);
		}
	}

	if (lightStrike)
	{
		if (lightRapidsCounter < maxRapids)
		{
			constexpr float duration = 0.35f;
			static float currentStrikeRadius = radius;

			if (currentStrikeRadius < radius)
			{
				currentStrikeRadius += (radius / duration) * memory::interfaces::globalVars->m_frametime;
			}
			else
			{
				currentStrikeRadius = 0.0f;
				lightRapidsCounter += 1;

				BeamInfo_t info{ };
				info.m_type = TE_BEAMPOINTS;
				info.m_flags = FBEAM_FADEOUT | FBEAM_ONLYNOISEONCE;
				info.m_modelName = "sprites/bluelightning.vmt";
				info.m_modelIndex = memory::interfaces::modelInfo->getModelIndex("materials/sprites/bluelightning.vmt");
				info.m_haloIndex = -1;
				info.m_haloScale = 0.0f;
				info.m_life = duration + 0.2f;
				info.m_width = 70.0f;
				info.m_endWidth = 2.0f;
				info.m_fadeLength = 1.0f;
				info.m_amplitude = 20.0f;
				info.m_red = 205;
				info.m_green = 180;
				info.m_blue = 255;
				info.m_brightness = 255;
				info.m_speed = 0.0f;
				info.m_startFrame = 0;
				info.m_frameRate = 1.0f;
				const float randomX = Random::getRandom(200.0f, 800.0f);
				const float randomY = Random::getRandom(200.0f, 800.0f);

				const Vec3 directionVector = math::angleVec(game::localPlayer->absAngles());
				const Vec3 localHead = game::localPlayer->getHitboxPos(HITBOX_HEAD);

				info.m_vecStart = Vec3
				{
					localHead[0] + (directionVector[0] * randomX),
					localHead[1] + (directionVector[1] * randomY),
					localHead[2] + 2000.0f // hope its fine 
				};
				info.m_vecEnd = Vec3
				{
					localHead[0] + (directionVector[0] * randomX),
					localHead[1] + (directionVector[1] * randomY),
					game::localPlayer->absOrigin()[2]
				};
				info.m_segments = 35;
				info.m_renderable = true;

				Beam_t* strike = memory::interfaces::beams->createBeamPoints(info);
			}

			lightStrike->m_radius = currentStrikeRadius;
		}
		else
		{
			lightRapidsCounter = 0;
			lightStrike->m_die = memory::interfaces::globalVars->m_curtime;
			lightStrike = nullptr;
		}
	}
}

void weather::precipitation::reset()
{
	// engine does it
	//shutdown();
	weatherFields.ent = nullptr;

	newMap = true;

	lastTimeStorm = memory::interfaces::globalVars->m_curtime + Random::getRandom(2.0f, 5.0f);
	lightRapidsCounter = 0;
}

void weather::precipitation::shutdown()
{
	if (weatherFields.ent)
	{
		// if executed automatically, then remove this entry sound at all
		if (globals::isShutdown)
		{
			const helper::CustomPrecipitationType_t precipType = helper::configToPrecip(vars::visuals->world->weather->type);
			if (precipType != helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE)
			{
				memory::interfaces::engineSound->emitAmbientSound(pickSoundForPrecip(precipType).data(),
					vars::visuals->world->weather->volume, PITCH_NORM, SND_STOP);
			}
		}

		//memory::gPrecipitations().FindAndRemove(weatherFields.ent);		 
		memory::precipDestruct()(weatherFields.ent);
		weatherFields.ent = nullptr;
		memory::interfaces::physicsCollision->vcollideUnload(&weatherFields.collide);
		if(lightStrike)
			lightStrike->m_die = memory::interfaces::globalVars->m_curtime;
		lightStrike = nullptr;
		done = false;
	}
}

void weather::precipitation::loadCollide()
{
	// weather solid counts is always 1, buffer is about 530-550 size with always very similar data, see dumps
	memory::interfaces::physicsCollision->vcollideLoad(&weatherFields.collide, 1, reinterpret_cast<const char*>(weatherBuffer.data()), weatherBuffer.size(), false);
}

vcollide_t& weather::precipitation::getCollide()
{
	return weatherFields.collide;
}

Precipitation_t* weather::precipitation::getPrecip()
{
	if (!done)
		return nullptr;

	return weatherFields.ent;
}

std::string_view weather::precipitation::pickSoundForPrecip(helper::CustomPrecipitationType_t precip)
{
	// some sounds may not play in a loop

	switch (precip)
	{
	case helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_RAIN:
		return "ambient/weather/aztec_rain_lp_01.wav";
	case helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_SNOW:
		return "ambient/wind/lightwind_02.wav";
	case helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN:
		return "ambient/weather/rumble_rain_nowind.wav";
	case helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM:
		return "ambient/weather/rumble_rain.wav";
	case helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW:
		return "ambient/wind/windgust.wav";
	default:
		return "";
	}
}

std::string_view weather::precipitation::pickRandomSoundForThunder()
{
	constexpr std::array allTHunders
	{
		"ambient/weather/thunder1.wav",
		"ambient/weather/thunder2.wav",
		"ambient/weather/thunder3.wav",
		"ambient/weather/thunderstorm/lightning_strike_1.wav",
		"ambient/weather/thunderstorm/lightning_strike_2.wav",
		"ambient/weather/thunderstorm/lightning_strike_3.wav",
		"ambient/weather/thunderstorm/lightning_strike_4.wav"
	};

	return allTHunders.at(Random::getRandom<size_t>(0, allTHunders.size() - 1));
}