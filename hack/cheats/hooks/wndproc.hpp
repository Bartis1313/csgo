#pragma once

#include <Windows.h>

#include "helpers/common.hpp"

_HOOK_BEGIN
class wndProcSys
{
	ADD_HOOK_FRIEND;
public:
	inline static LRESULT wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

	static void init();
	static void shutdown();

	inline static WNDPROC wndProcOriginal = nullptr;
	inline static HWND currentWindow = nullptr;
protected:
	static void registerRun(const std::function<void()>& func) { Storage::runs.push_back(func); }
private:
	struct Storage
	{
		inline static vectorRunnable<std::function<void()>> runs;
	};
};
_HOOK_END