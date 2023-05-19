#pragma once

class Player_t;

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

	[[nodiscard]] bool isBlacklisted(Player_t* ent);
	[[nodiscard]] BlacklistedPlayer getGuid(Player_t* ent);

	void init();
	void shutdown();
	void reset();

	void add(Player_t* ent);
	void remove(Player_t* ent);
};