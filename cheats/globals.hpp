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
	inline int screenX = 0;
	inline int screenY = 0;
	inline bool interfacesDone = false;
	// TODO: store HWND by thread
	inline HWND csgoWHND;
}