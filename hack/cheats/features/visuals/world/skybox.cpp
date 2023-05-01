#include "skybox.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>
#include <menu/GUI-ImGui/selections.hpp>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/helper/initable.hpp>

namespace
{
	struct SkyboxHandler : hooks::FrameStageNotify
	{
		SkyboxHandler()
		{
			this->registerInit(skybox::init);
			this->registerRun(skybox::run);
			this->registerShutdown(skybox::shutdown);
		}
	} skyboxHandler;
}

namespace skybox
{
	void loadSkybox(const std::string& name);
	void checkCustomSkybox();

	std::filesystem::path m_pathCustomSkybox;
	IConVar* sv_skyname;
}

void skybox::init()
{
	checkCustomSkybox();
	reloadCustomSkyboxes();

	sv_skyname = memory::interfaces::cvar->findVar("sv_skyname");
}

void skybox::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!changedState)
		return;

	if (!game::isAvailable())
		return;

	// bool to point true = custom
	std::pair<int, bool> keySky;
	if (auto custom = vars::visuals->world->sky->indexCustom; custom > 0)
		keySky = { custom, true };
	else
		keySky = { vars::visuals->world->sky->indexNormal, false };

	if (keySky.first != 0 && !globals::isShutdown) // is not none and there is no shutdown
	{
		keySky.second == true
			? loadSkybox(customSkyboxes.at(keySky.first).c_str())
			: loadSkybox(selections::skyboxes.at(keySky.first));
	}
	else
	{
		// restore the sky
		loadSkybox(sv_skyname->m_valueNow);
	}
}

void skybox::loadSkybox(const std::string& name)
{
	memory::loadSky()(name.c_str());
}

void skybox::checkCustomSkybox()
{
	auto path = std::filesystem::current_path() / "csgo" / "materials" / "skybox";

	if (!std::filesystem::exists(path))
	{
		if (!std::filesystem::create_directories(path))
			return;
	}

	m_pathCustomSkybox = path;
}

void skybox::reloadCustomSkyboxes()
{
	customSkyboxes.clear();
	customSkyboxes.emplace_back("none");
	auto iterator = std::filesystem::directory_iterator(m_pathCustomSkybox);
	for (const auto& entry : iterator)
	{
		if (std::string name = entry.path().filename().string();
			entry.path().extension() == ".vtf" && !name.empty())
		{
			customSkyboxes.push_back(name);
		}
	}
}

void skybox::shutdown()
{
	loadSkybox(sv_skyname->m_valueNow);
}
