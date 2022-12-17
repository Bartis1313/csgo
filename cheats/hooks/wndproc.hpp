#pragma once

#include <Windows.h>

struct wndProcSys
{
	inline static WNDPROC wndProcOriginal = nullptr;
	inline static HWND currentWindow = nullptr;
	inline static LRESULT wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

	static void init();
	static void shutdown();
};