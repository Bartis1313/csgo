#pragma once

#include "helpers/common.hpp"

#include <SDK/CCommonHostState.hpp>

_HOOK_BEGIN
class R_DecalCreate
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_CDECL*)(int, int, int, int, int, char, char);

	static value HACK_CDECL hook(int a1, int a2, int a3, int a4, int a5, char a6, char a7);
	inline static call original;
};
_HOOK_END