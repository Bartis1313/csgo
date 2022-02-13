#pragma once
#include "../../../SDK/interfaces/interfaces.hpp"

namespace legitbot
{
	void drawFov();
	void run(CUserCmd* cmd);
	void RCS(CUserCmd* cmd);
	void runRCS(CUserCmd* cmd);

	inline Player_t* bestEnt = nullptr;
}