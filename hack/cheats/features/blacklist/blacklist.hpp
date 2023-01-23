#pragma once

#include <cheats/classes/onlyInit.hpp>

#include <nlohmann/json.hpp>

class Player_t;
struct PlayerInfo_t;

class Blacklist : protected OnlyInitType
{
public:
	Blacklist() :
		OnlyInitType{}
	{}

	struct BlacklistedPlayer
	{
		[[nodiscard]] constexpr bool operator==(const BlacklistedPlayer& rhs) const
		{
			return this->m_guid == rhs.m_guid && this->m_bot == rhs.m_bot;
		}

		uint64_t m_guid;
		bool m_bot;
		std::string m_playerName;
	};

	void add(Player_t* ent);
	void remove(Player_t* ent);
	bool isBlacklisted(Player_t* ent) const;
	BlacklistedPlayer getGuid(Player_t* ent) const;
protected:
	virtual void init() override;
	virtual void shutdown() override;
	virtual void reset() override;
private:
	bool loadCfg();
	bool saveCfg();
	std::filesystem::path getPathForConfig() const;
	std::string getCorrectKey(const BlacklistedPlayer& player) const;

	std::vector<BlacklistedPlayer> m_allGuids;
	std::string_view m_folderName;
	std::filesystem::path m_saveDir;
	nlohmann::json m_json;
};

GLOBAL_FEATURE(Blacklist);