#include "blacklist.hpp"

#include <config/config.hpp>
#include <config/jsonExtended.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>

#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

using json = nlohmann::json;

void from_json(const json& j, Blacklist::BlacklistedPlayer& val)
{
	from_json(j, "Guid", val.m_guid);
	from_json(j, "Bot", val.m_bot);
	from_json(j, "Name", val.m_playerName);
}

void to_json(json& j, const Blacklist::BlacklistedPlayer& val)
{
	j["Guid"] = val.m_guid;
	j["Bot"] = val.m_bot;
	j["Name"] = val.m_playerName;
}

void Blacklist::init()
{
	m_folderName = "blacklist";
	m_saveDir = config.getHackPath() / config.getExtraLoadPath() / m_folderName / getPathForConfig();

	loadCfg();
}

void Blacklist::reset()
{
	if (m_allGuids.empty())
		return;

	m_allGuids.erase(std::remove_if(m_allGuids.begin(), m_allGuids.end(), [this](const BlacklistedPlayer& player)
		{
			if (player.m_bot)
			{
				m_json.erase(m_json.find(getCorrectKey(player)));
				return true;
			}

	return false;
		})
	);
}

bool Blacklist::loadCfg()
{
	if (auto path = config.getHackPath() / config.getExtraLoadPath() / m_folderName; !std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	std::ifstream input{ m_saveDir };
	if (!input)
		return false;

	if (!std::filesystem::is_empty(m_saveDir))
	{
		m_json = json::parse(input);

		for (const auto& [key, value] : m_json.items())
		{
			BlacklistedPlayer player;
			from_json(value, player);

			m_allGuids.push_back(player);
		}
	}

	return true;
}

bool Blacklist::saveCfg()
{
	std::ofstream out{ m_saveDir };
	if (!out)
		return false;

	json j;
	for (const auto& el : m_allGuids)
		to_json(j[getCorrectKey(el)], el);

	if (!j.empty())
		m_json.update(j);

	out << std::setw(4) << m_json;

	return true;
}

// for performance
void Blacklist::shutdown()
{
	if (!saveCfg())
		console::error("Saving config has failed");
}

bool Blacklist::isBlacklisted(Player_t* ent) const
{
	const auto guid = getGuid(ent);
	auto itr = std::ranges::find_if(m_allGuids, [guid](const BlacklistedPlayer& player)
		{
			return player == guid;
		}
	);

	return itr != m_allGuids.end();
}

void Blacklist::add(Player_t* ent)
{
	const auto guid = getGuid(ent);
	if (!isBlacklisted(ent))
	{
		m_allGuids.push_back(guid);
		json j;
		to_json(j[getCorrectKey(guid)], guid);
		m_json.update(j);
	}
	else
		console::warn("Player already blacklisted! guid: {} isBot {}", guid.m_guid, guid.m_bot);
}

void Blacklist::remove(Player_t* ent)
{
	const auto guid = getGuid(ent);
	m_allGuids.erase(std::remove_if(m_allGuids.begin(), m_allGuids.end(), [this, guid](const BlacklistedPlayer& player)
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

std::filesystem::path Blacklist::getPathForConfig() const
{
	std::filesystem::path path(m_folderName);
	if (path.extension() != ".json")
		path.replace_extension(".json");

	return path;
}

std::string Blacklist::getCorrectKey(const BlacklistedPlayer& player) const
{
	auto key = player.m_bot
		? std::format("{} BOT", player.m_guid)
		: std::format("{}", player.m_guid);

	return key;
}

Blacklist::BlacklistedPlayer Blacklist::getGuid(Player_t* ent) const
{
	PlayerInfo_t info;
	memory::interfaces::engine->getPlayerInfo(ent->getIndex(), &info);
	auto guid = info.m_fakePlayer
		? BlacklistedPlayer{ .m_guid = static_cast<uint64_t>(info.m_userID), .m_bot = true, .m_playerName = std::string{ info.m_name } }
	: BlacklistedPlayer{ .m_guid = info.m_steamID64, .m_bot = false, .m_playerName = std::string{ info.m_name } };

	return guid;
}