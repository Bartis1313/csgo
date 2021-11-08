#pragma once
#include "../../game.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../menu/vars.hpp"
#include "../../../SDK/math/Vector.hpp"

namespace legitbot
{
	Vector getBestBonePos(CUserCmd* cmd);
	void drawFov();
	//void drawPoint();
	// TODO: fix drop fps issue
	void run(CUserCmd* cmd);
	void RCS(CUserCmd* cmd);

	inline Player_t* bestEnt = nullptr;
	inline Vector bestPoint = {};
	//inline bool aimbotWorking = false;
}