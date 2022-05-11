#pragma once

#include <Windows.h>
#include "../SDK/math/Vector.hpp"

namespace globals
{
	inline float FOV = 90.0f;
	inline Vector realAngle = {};
	inline Vector fakeAngle = {};
	inline size_t shotsFired = 0;
	inline size_t shotsHit = 0;
	inline size_t shotsHead = 0;
	inline HMODULE instance = NULL;
	inline int screenX = 0;
	inline int screenY = 0;
	inline int mouseX = 0;
	inline int mouseY = 0;
	inline HWND csgoWHND = NULL;
	inline bool isShutdown = false;
	inline bool isInHotkey = false;
}