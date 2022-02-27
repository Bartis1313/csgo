#pragma once
#include "../SDK/math/Vector.hpp"

namespace globals
{
	inline float FOV = 90.0f;
	inline Vector realAngle = {};
	inline Vector fakeAngle = {};
	inline int shotsFired = 0;
	inline int shotsHit = 0;
	inline HMODULE instance = {};
	inline int mouseX = 0;
	inline int mouseY = 0;
	inline bool keyState[256];
	inline bool previousKeyState[256];
	inline int screenX = 0;
	inline int screenY = 0;
	inline bool interfacesDone = false;
}