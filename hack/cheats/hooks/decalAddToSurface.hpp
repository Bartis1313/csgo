#pragma once

#include "helpers/common.hpp"

#include <SDK/CCommonHostState.hpp>

_HOOK_BEGIN
class R_AddDecalToSurface
{
	ADD_HOOK_FRIEND;
private:
	using value = int*;
	using call = value(HACK_CDECL*)(void*, void*, void*, void*);

	static value HACK_CDECL hook(void*, void*, void*, void*);
	inline static call original;
};
_HOOK_END