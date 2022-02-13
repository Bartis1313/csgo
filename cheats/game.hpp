#pragma once
#include "../SDK/structs/Entity.hpp"

namespace game
{
	inline Player_t* localPlayer = nullptr;
	_NODISCARD bool isAvailable();
	float serverTime(CUserCmd* cmd = nullptr);
}