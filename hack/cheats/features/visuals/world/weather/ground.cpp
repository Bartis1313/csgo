#include "ground.hpp"

#include "helper.hpp"

#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/CCommonHostState.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientState.hpp>
#include <SDK/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>

#include <config/vars.hpp>
#include <utilities/console/console.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/levelInitPreEntity.hpp>
#include <cheats/features/visuals/chams/factory/factory.hpp>


// artifacts is thta manual solution won't reload texture sounds, decals coming from them etc...
// also material vars dont apply to it, dont know why, even though they are precached once again
// thanks ze-elite for good texture restoring method

namespace
{
	struct GroundFSN : hooks::FrameStageNotify
	{
		GroundFSN()
		{
			this->registerInit(weather::ground::init);
			this->registerRun(weather::ground::resetFindMat);
		}
	} groundFSN;

	struct GroundRoundReset : hooks::LevelInitPreEntity
	{
		GroundRoundReset()
		{
			this->registerReset(weather::ground::reset);
		}
	} groundRoundReset;
}

namespace weather::ground
{
	struct MatSaver
	{
		std::string_view name;
		std::string_view varName;
		Vec3 varValue;
	};

	std::vector<std::pair<int, IMaterial*>> oldMaterialsManualPaired;
	std::vector<std::string_view> oldMaterialsFindMat;
	std::vector<MatSaver> oldMaterialsFindMatVars;
	std::vector<std::string_view> oldMaterialsAll;
}

void weather::ground::init()
{

}

void weather::ground::reset()
{
	oldMaterialsManualPaired.clear();
}

// this is not called automatically. it's hard to make it working with not destroying the context too soon and magically tell matsys to wait for x time
void weather::ground::shutdown()
{
	if (!calledShutdown)
		return;

	// because this is only important ofr mid-round shutdown
	if (!game::isAvailable())
		return;

	if (!oldMaterialsFindMatVars.empty())
	{
		for (int i = 0; i < memory::interfaces::hostState->worldbrush.numtexinfo; ++i)
		{
			auto mat = memory::interfaces::hostState->worldbrush.textinfo[i].material;

			if (auto it = std::ranges::find_if(oldMaterialsFindMatVars, [mat](const MatSaver& matSaver)
				{
					if (std::string_view{ mat->getName() } == matSaver.name)
						return true;

					return false;
				}
			); it != oldMaterialsFindMatVars.end())
			{
				mat->findVar("$envmap")->setValue(it->varName.data());
				mat->findVar("$envmaptint")->setValues(it->varValue);
			}
		}

		oldMaterialsFindMatVars.clear();
	}

	if (!oldMaterialsFindMat.empty())
	{
		// how 2 sort dis, not names, indexes how?
		for (int i = 0; i < memory::interfaces::hostState->worldbrush.numtexinfo; ++i)
		{
			auto mat = memory::interfaces::hostState->worldbrush.textinfo[i].material;

		}

		//memory::restoreMaterialSystemObjects()(0x1);

		oldMaterialsFindMat.clear();

		return;
	}

	if (!oldMaterialsManualPaired.empty())
	{
		// manual ones will get renamed, we replaced pointer, so we need to reassign it
		for (int i = 0; i < memory::interfaces::hostState->worldbrush.numtexinfo; ++i)
		{
			for (auto& [index, oldMaterial] : oldMaterialsManualPaired)
			{
				if (i == index)
				{
					memory::interfaces::hostState->worldbrush.textinfo[i].material = oldMaterial;
				}
			}
		}
		oldMaterialsManualPaired.clear();

		memory::interfaces::matSys->forceSingleThreaded();
		memory::restoreMaterialSystemObjects()(0x1);

		return;
	}

	calledShutdown = false;
}

void weather::ground::resetFindMat(FrameStage stage)
{
	// ok we are not on serv yet, or we cleared mats already
	if (oldMaterialsFindMat.empty())
		return;

	// do only if we quit the game
	if (!game::isAvailable())
		oldMaterialsFindMat.clear();
}

// we won't care for wet ground here, it is so far not working with anything... vars cant get saved for cubemap on mid-round, and touching files manually is bad
void weather::ground::applyManually()
{
	if (!game::isAvailable())
		return;

	if (calledShutdown)
	{
		ImNotify::add(ImNotify{ 3.0, "Restored textures", "If you called them after findmat - they won't change" });
		shutdown();
	}

	if (!oldMaterialsFindMatVars.empty())
		return;

	create();
}

void weather::ground::create()
{
	if (!game::isAvailable())
		return;

	if (!calledCreate)
		return;

	const auto& groundInfo = getGroundInfo();
	auto mapIt = groundInfo.find(memory::interfaces::engine->getLevelName());
	if (mapIt == groundInfo.end())
	{
		//ImNotify::add(ImNotify{ 3.0, "Can't create textures", "You are on unsupported map, add keys by yourself" });
		return;
	}

	ImNotify::add(ImNotify{ 3.0, "Created textures manually", "You can restart them without issues" });

	const static auto materialSnow = material::factory::findMaterial("ground/snow01", TEXTURE_GROUP_WORLD);

	for (int i = 0; i < memory::interfaces::hostState->worldbrush.numtexinfo; ++i)
	{
		auto& mat = memory::interfaces::hostState->worldbrush.textinfo[i].material;
		std::string_view materialName = memory::interfaces::hostState->worldbrush.textinfo[i].material->getName();
		auto mapIt = groundInfo.find(memory::interfaces::engine->getLevelName());
		if (mapIt != groundInfo.end())
		{
			if (std::string_view{ mat->getTextureGroupName() } == TEXTURE_GROUP_WORLD)
			{
				const auto& textures = mapIt->second;
				if (std::ranges::find_if(textures, [materialName](std::string_view textureFilter)
					{
						return materialName.contains(textureFilter);
					}) != textures.end())
				{
					oldMaterialsManualPaired.emplace_back(std::make_pair(i, mat));

					memory::interfaces::hostState->worldbrush.textinfo[i].material = materialSnow;
				}
			}
		}
	}

	memory::interfaces::matSys->forceSingleThreaded();
	memory::restoreMaterialSystemObjects()(0x1);

	calledCreate = false;
}

IMaterial* weather::ground::applyFindMat(IMaterial* mat)
{
	if (globals::isShutdown)
		return nullptr;

	if (!vars::visuals->world->weather->useGround)
		return nullptr;

	const auto type = helper::configToPrecip(vars::visuals->world->weather->type);

	if (type != helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN
		&& type != helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM
		&& type != helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW)
		return nullptr;

	// grab only on map load
	if (memory::interfaces::clientState->m_deltaTick != -1)
		return nullptr;

	const auto& groundInfo = getGroundInfo();
	const std::string_view materialName = mat->getName();

	auto mapIt = groundInfo.find(memory::levelName()); // fastest way
	if (mapIt != groundInfo.end())
	{
		if (std::string_view{ mat->getTextureGroupName() } == TEXTURE_GROUP_WORLD)
		{
			const auto& textures = mapIt->second;
			if (std::ranges::find_if(textures, [materialName](std::string_view textureFilter)
				{
					return materialName.contains(textureFilter);
				}) != textures.end())
			{
				if (type == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN || type == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM)
				{
					const auto envmap = mat->findVar("$envmap");
					const auto envmaptint = mat->findVar("$envmaptint");

					const auto nameMap = envmap->getString();
					const auto valuesMap = envmap->getVector();

					oldMaterialsFindMatVars.emplace_back(MatSaver{ materialName, nameMap, Vec3{ valuesMap[0], valuesMap[1], valuesMap[2] } });

					envmap->setValue("env_cubemap");
					envmaptint->setValues(Vec3{ 1.0f, 1.0f, 1.0f });
				}
				else if (type == helper::CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW)
				{
					oldMaterialsFindMat.push_back(materialName);

					const static auto materialSnow = material::factory::findMaterial("ground/snow01", TEXTURE_GROUP_WORLD);
					return materialSnow;
				}

				return mat;
			}
		}
	}

	return nullptr;
}

weather::ground::groundInfo weather::ground::getGroundInfo()
{
	static groundInfo gInfo;
	gInfo["de_mirage"] =
	{
		"maps/de_mirage/de_mirage",
		"de_mirage/ground",
		"de_mirage/tile",
		"concrete/street_overlay_parkingstripe",
		"de_dust/sitebwall08a",
		"de_dust/tilefloor",
		"de_dust/stonestep",
		"maps/de_mirage/de_dust/tilefloor",
		"wood/woodfloor",
		"marble",
		"concrete/blend_blacktopsand_01",
		"concrete/concrete_floor_04",
		"wood/plywood02",
		"wood/milbeams",
		"wood/woodbeam001"
	};

	return gInfo;
}