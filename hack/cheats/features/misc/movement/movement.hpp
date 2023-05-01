#pragma once

#include <SDK/math/Vector.hpp>

class CUserCmd;
class IConVar;

namespace movement
{
	void run(CUserCmd* cmd);
	void init();

	void fixMovement(CUserCmd* cmd, const Vec3& oldAngle);
}