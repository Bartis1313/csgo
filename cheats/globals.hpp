#pragma once
#include "../SDK/math/Vector.hpp"

namespace globals
{
	inline float FOV = 90;
	inline Vector realAngle = { 0, 0, 0 };
	inline Vector fakeAngle = { 0, 0, 0 };
	inline int shotsFired = 0;
	inline int shotsHit = 0;
	inline HMODULE instance = {};
}