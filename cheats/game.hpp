#pragma once
#include "../SDK/structs/Entity.hpp"

namespace game
{
	inline Player_t* localPlayer = nullptr;
	[[nodiscard]] bool isAvailable();
	float serverTime(CUserCmd* cmd = nullptr);
	[[nodiscard]] int getLocalKills();
	[[nodiscard]] int getLocalDeaths();
	[[nodiscard]] int getLocalPing();
}