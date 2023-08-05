#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class setDrawColor
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int, int, int, int);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int r, int g, int b, int a);
	inline static call original;
};
_HOOK_END