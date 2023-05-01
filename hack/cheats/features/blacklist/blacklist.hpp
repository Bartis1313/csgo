#pragma once

#include <nlohmann/json.hpp>

class Player_t;
struct PlayerInfo_t;

namespace blacklist
{
	struct BlacklistedPlayer
	{
		[[nodiscard]] constexpr bool operator==(const BlacklistedPlayer& rhs) const
		{
			return this->guid == rhs.guid && this->bot == rhs.bot;
		}

		bool bot;
		uint64_t guid;
		std::string playerName;
	};

	void add(Player_t* ent);
	void remove(Player_t* ent);
	bool isBlacklisted(Player_t* ent);
	BlacklistedPlayer getGuid(Player_t* ent);

	void init();
	void shutdown();
	void reset();

	bool loadCfg();
	std::filesystem::path getPathForConfig();
	std::string getCorrectKey(const BlacklistedPlayer& player);

	inline std::vector<BlacklistedPlayer> m_allGuids;
	inline std::string_view m_folderName;
	inline std::filesystem::path m_saveDir;
	inline nlohmann::json m_json;
};