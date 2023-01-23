#pragma once

#include <Windows.h>
#include <vector>

namespace globals
{
	inline float FOV = 90.0f;
	inline HMODULE instance = NULL;
	inline int screenX = 0;
	inline int screenY = 0;
	inline int mouseX = 0;
	inline int mouseY = 0;
	inline HWND csgoWHND = NULL;
	inline bool isShutdown = false;
	inline bool isInHotkey = false;
}