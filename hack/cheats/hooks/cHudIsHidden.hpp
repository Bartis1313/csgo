#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class CHudIsHidden
{
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int flags);
	inline static call original;
};
_HOOK_END