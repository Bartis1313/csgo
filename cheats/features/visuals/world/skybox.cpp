#include "skybox.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>
#include <menu/GUI-ImGui/selections.hpp>

void SkyboxEdit::init()
{
	checkCustomSkybox();
	reloadCustomSkyboxes();
}

void SkyboxEdit::run(int frame)
{
	if (frame != FRAME_RENDER_START)
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
			? loadSkybox(m_allCustomSkyboxes.at(keySky.first).c_str())
			: loadSkybox(selections::skyboxes.at(keySky.first));
	}
	else
	{
		// restore the sky
		const static auto oldSky = interfaces::cvar->findVar(XOR("sv_skyname"));
		loadSkybox(oldSky->m_string);
	}
}

void SkyboxEdit::loadSkybox(const std::string& name)
{
	g_Memory.m_loadSky()(name.c_str());
}

void SkyboxEdit::checkCustomSkybox()
{
	auto path = std::filesystem::current_path() / XOR("csgo") / XOR("materials") / XOR("skybox");

	if (!std::filesystem::exists(path))
	{
		if (!std::filesystem::create_directories(path))
			return;
	}

	m_pathCustomSkybox = path;
}

void SkyboxEdit::reloadCustomSkyboxes()
{
	m_allCustomSkyboxes.clear();
	m_allCustomSkyboxes.emplace_back(XOR("none"));
	auto iterator = std::filesystem::directory_iterator(m_pathCustomSkybox);
	for (const auto& entry : iterator)
	{
		if (std::string name = entry.path().filename().string();
			(entry.path().extension() == XOR(".vmt") || entry.path().extension() == XOR(".vtf")) && !name.empty())
		{
			m_allCustomSkyboxes.push_back(name);
		}
	}
}
