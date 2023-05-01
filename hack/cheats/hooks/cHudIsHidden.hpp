#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class CHudIsHidden
{
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(THISCALL*)(THISPTR, int);

	static value FASTCALL hook(FAST_ARGS, int flags);
	inline static call original;
};
_HOOK_END