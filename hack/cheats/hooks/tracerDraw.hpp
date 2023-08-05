#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class TracerDraw
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_STDCALL*)(void*, void*, void*, float, float*);

	static value HACK_STDCALL hook(void* mesh, void* start, void* end, float w, float* color);
	inline static call original;
};
_HOOK_END