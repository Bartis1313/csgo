#include "discord.hpp"

#include <discord_register.h>
#include <discord_rpc.h>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/CGlobalVars.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <cstring>
#include <chrono>
#include <unordered_map>

namespace
{
	constexpr std::string_view dcKey{ "990623614527213578" };
	constexpr std::string_view appID{ "730" }; // GetAppID -> 730
}

void discord::init()
{
	Discord_Initialize(dcKey.data(), NULL, 1, appID.data());
}

static std::pair<std::string, std::string> getImage(const std::string& map)
{
	static std::unordered_map<std::string, std::string> mapNames = // my images under this app I have added
	{
		{ "de_dust2", "dust2" },
		{ "de_mirage", "mirage" },
		{ "de_inferno", "inferno" },
		{ "de_vertigo", "vertigo" },
		{ "de_cobble", "cobble" },
		{ "de_cache", "cache" },
		{ "de_iris", "iris" },
		{ "de_ancient", "ancient" },
		{ "de_train", "train" },
		{ "de_overpass", "overpass" },
		{ "de_nuke", "nuke" },
		{ "de_canals", "canals" },
		{ "de_climb", "climb" },
		{ "de_agency", "agency" },
		{ "cs_militia", "milita" },
		{ "de_office", "office" },
		{ "de_italy", "italy" },
		{ "de_assault", "assault" }
	};

	if (auto ok = mapNames.find(map); ok == mapNames.end())
		return std::make_pair("unknown", "unknown");
	else
		return std::make_pair(ok->first, ok->second);
}

void discord::run()
{
	static bool didReset{ false };

	if (!vars::misc->discord->enabled)
	{
		if (!didReset)
		{
			Discord_ClearPresence();
			didReset = true;
		}

		return;
	}

	didReset = false;

	DiscordRichPresence pres{ };
	std::memset(&pres, 0, sizeof(pres));

	static std::string state{ "State Unknown" };
	static std::pair<std::string, std::string> image{ };

	if (!memory::interfaces::engine->isInGame())
	{
		state = "In menu";
		image = getImage("");
	}
	else if (game::isAvailable())
	{
		auto map = memory::interfaces::engine->getLevelName();

		state = std::format("On map: {}", map);
		image = getImage(map);
	}

	pres.state = state.c_str();

	static std::string details{ "Unknown details" };
	if (game::isAvailable())
	{
		if (auto wpn = game::localPlayer->getActiveWeapon(); wpn)
		{
			std::string_view pos = game::localPlayer->m_szLastPlaceName();
			if (pos.empty())
				pos = "null";
			details = std::format("Weapon: {} Pos: {}", wpn->getWpnName(), pos);
		}
	}

	pres.details = details.c_str();
	const static auto epoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	pres.startTimestamp = epoch;

	pres.largeImageKey = image.second.c_str();
	pres.largeImageText = image.first.c_str();

	Discord_UpdatePresence(&pres);
}

void discord::shutdown()
{
	Discord_ClearPresence();
	Discord_Shutdown();
}