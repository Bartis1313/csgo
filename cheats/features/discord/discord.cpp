#include "discord.hpp"

#include <discord_register.h>
#include <discord_rpc.h>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/CGlobalVars.hpp>
#include <config/vars.hpp>
#include <game/game.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <cstring>
#include <chrono>
#include <unordered_map>

void DiscordPresence::init()
{
	Discord_Initialize(XOR("990623614527213578"), NULL, 1, XOR("730")); // GetAppID -> 730
}

static std::pair<std::string, std::string> getImage(const std::string& map)
{
#define AM(f, s) { XOR(f), XOR(s) }
	static std::unordered_map<std::string, std::string> mapNames = // my images under this app I have added
	{
		AM("de_dust2", "dust2"),
		AM("de_mirage", "mirage"),
		AM("de_inferno", "inferno"),
		AM("de_vertigo", "vertigo"),
		AM("de_cobble", "cobble"),
		AM("de_cache", "cache"),
		AM("de_iris", "iris"),
		AM("de_ancient", "ancient"),
		AM("de_train", "train"),
		AM("de_overpass", "overpass"),
		AM("de_nuke", "nuke"),
		AM("de_canals", "canals"),
		AM("de_climb", "climb"),
		AM("de_agency", "agency"),
		AM("cs_militia", "milita"),
		AM("de_office", "office"),
		AM("de_italy", "italy"),
		AM("de_assault", "assault"),
	};
#undef AM

	if (auto ok = mapNames.find(map); ok == mapNames.end())
		return std::make_pair(XOR("unknown"), XOR("unknown"));
	else
		return std::make_pair(ok->first, ok->second);
}

void DiscordPresence::run()
{
	if (!vars::misc->discord->enabled)
		return;

	DiscordRichPresence pres;
	std::memset(&pres, 0, sizeof(pres));

	static std::string state;

	static std::pair<std::string, std::string> image;

	if (!interfaces::engine->isInGame())
	{
		state = XOR("In menu");
		image = getImage("");
	}
	else if (game::isAvailable())
	{
		auto map = interfaces::engine->getLevelName();

		state = FORMAT(XOR("On map: {}"), map);
		image = getImage(map);
	}

	pres.state = state.c_str();

	static std::string details;
	if (game::isAvailable())
	{
		if (auto wpn = game::localPlayer->getActiveWeapon(); wpn)
		{
			std::string_view pos = game::localPlayer->m_szLastPlaceName();
			if (pos.empty())
				pos = XOR("null");
			details = FORMAT(XOR("Weapon: {} Pos: {}"), wpn->getWpnName(), pos);
		}
	}
	else
		details = XOR("Unknown details");

	pres.details = details.c_str();
	const static auto epoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	pres.startTimestamp = epoch;

	pres.largeImageKey = image.second.c_str();
	pres.largeImageText = image.first.c_str();

	Discord_UpdatePresence(&pres);
}

void DiscordPresence::shutdown()
{
	Discord_ClearPresence();
	Discord_Shutdown();
}