#pragma once
#include "../../game.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../menu/vars.hpp"
#include "../../../SDK/math/Vector.hpp"
#include <map>

namespace legitbot
{
	void drawFov();
	void run(CUserCmd* cmd);
	void RCS(CUserCmd* cmd);

	inline Player_t* bestEnt = nullptr;
}