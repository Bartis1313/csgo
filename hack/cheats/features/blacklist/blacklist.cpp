#include "blacklist.hpp"

#include <config/config.hpp>
#include <config/jsonExtended.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>

#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

#include <cheats/helper/initable.hpp>
#include <cheats/helper/shutdownable.hpp>
#include <cheats/hooks/helpers/common.hpp>
#include <cheats/hooks/levelInitPreEntity.hpp>

namespace
{
	struct BlacklistInit : InitAble
	{
		BlacklistInit()
		{
			this->registerInit(blacklist::init);
		}
	} blacklistInit;

	struct BlacklistShutdown : ShutdownAble
	{
		BlacklistShutdown()
		{
			this->registerShutdown(blacklist::shutdown);
		}
	} blacklistShutdown;

	struct BlacklistReset : hooks::LevelInitPreEntity
	{
		BlacklistReset()
		{
			this->registerReset(blacklist::reset);
		}
	} blacklistReset;
}

namespace blacklist
{
	bool loadCfg();
	std::filesystem::path getPathForConfig();
	std::string getCorrectKey(const BlacklistedPlayer& player);

	std::vector<BlacklistedPlayer> allGuids;
	constexpr std::string_view folderName{ "blacklist" };
	std::filesystem::path saveDir;
	nlohmann::json m_json;
}

using json = nlohmann::json;

void from_json(const json& j, blacklist::BlacklistedPlayer& val)
{
	from_json(j, "Guid", val.guid);
	from_json(j, "Bot", val.bot);
	from_json(j, "Name", val.playerName);
}

void to_json(json& j, const blacklist::BlacklistedPlayer& val)
{
	j["Guid"] = val.guid;
	j["Bot"] = val.bot;
	j["Name"] = val.playerName;
}

void blacklist::init()
{
	saveDir = api::getHackPath() / api::getExtraLoadPath() / folderName / getPathForConfig();

	loadCfg();

	console::debug("blacklist init");
}

// we only erase bots after the whole match has ended, players are left untouched
void blacklist::reset()
{
	if (allGuids.empty())
		return;

	allGuids.erase(std::remove_if(allGuids.begin(), allGuids.end(), [](const BlacklistedPlayer& player)
		{
			if (player.bot)
			{
				m_json.erase(m_json.find(getCorrectKey(player)));
				return true;
			}

			return false;
		})
	);
}

bool blacklist::loadCfg()
{
	if (auto path = api::getHackPath() / api::getExtraLoadPath() / folderName; !std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	std::ifstream input{ saveDir };
	if (!input)
		return false;

	if (!std::filesystem::is_empty(saveDir))
	{
		m_json = json::parse(input);

		for (const auto& [key, value] : m_json.items())
		{
			BlacklistedPlayer player{ };
			from_json(value, player);

			allGuids.push_back(player);
		}
	}

	return true;
}

bool saveCfg()
{
	std::ofstream out{ blacklist::saveDir };
	if (!out)
		return false;

	json j;
	for (const auto& el : blacklist::allGuids)
		to_json(j[blacklist::getCorrectKey(el)], el);

	if (!j.empty())
		blacklist::m_json.update(j);

	out << std::setw(4) << blacklist::m_json;

	return true;
}

// we will be saving only at end, because it doesn't really matter to save it instantly
void blacklist::shutdown()
{
	if (!saveCfg())
		console::error("Saving config has failed");
}

bool blacklist::isBlacklisted(Player_t* ent)
{
	const auto guid = getGuid(ent);
	auto itr = std::ranges::find_if(allGuids, [guid](const BlacklistedPlayer& player)
		{
			return player == guid;
		}
	);

	return itr != allGuids.end();
}

void blacklist::add(Player_t* ent)
{
	const auto guid = getGuid(ent);
	if (!isBlacklisted(ent))
	{
		allGuids.push_back(guid);
		json j;
		to_json(j[getCorrectKey(guid)], guid);
		m_json.update(j);
	}
	else
		console::warn("Player already blacklisted! guid: {} isBot {}", guid.guid, guid.bot);
}

void blacklist::remove(Player_t* ent)
{
	const auto guid = getGuid(ent);
	allGuids.erase(std::remove_if(allGuids.begin(), allGuids.end(), [guid](const BlacklistedPlayer& player)
		{
			if (player == guid)
			{
				m_json.erase(m_json.find(getCorrectKey(player)));
				return true;
			}

			return false;
		})
	);
}

std::filesystem::path blacklist::getPathForConfig()
{
	std::filesystem::path path{ folderName };
	if (path.extension() != ".json")
		path.replace_extension(".json");

	return path;
}

std::string blacklist::getCorrectKey(const blacklist::BlacklistedPlayer& player)
{
	auto key = player.bot
		? std::format("{} BOT", player.bot)
		: std::format("{}", player.guid);

	return key;
}

blacklist::BlacklistedPlayer blacklist::getGuid(Player_t* ent)
{
	PlayerInfo_t info{ };
	memory::interfaces::engine->getPlayerInfo(ent->getIndex(), &info);
	const auto guid = info.m_fakePlayer
		? BlacklistedPlayer{ .bot = true, .guid = static_cast<uint64_t>(info.m_userID), .playerName = std::string{ info.m_name } }
		: BlacklistedPlayer{ .bot = false, .guid = info.m_steamID64, .playerName = std::string{ info.m_name } };

	return guid;
}