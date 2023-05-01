#pragma once

#include "helpers/common.hpp"

#include <d3d9.h>

_HOOK_BEGIN
class Present
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = HRESULT;
	using call = value(STDCALL*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);

	static value STDCALL hook(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region);
	inline static call original;
	using clRun = std::function<void()>;
protected:
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
	};
};
_HOOK_END