#pragma once

#include <SDK/math/Vector.hpp>

class CUserCmd;

namespace rcs
{
	void init();
	void run(CUserCmd* cmd);
	void runMouse(float* x, float* y);

	inline Vec3 lastPunch{ };
}