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
	inline int mouseX = 0;
	inline int mouseY = 0;
	inline bool keyState[256];
	inline bool previousKeyState[256];
	// TODO: edit everything that calls vfunc for screensize and use this instead for less calls
	inline int screenX = 0;
	inline int screenY = 0;
}