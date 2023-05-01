#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class IsFollowingEntity
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(THISCALL*)(THISPTR);

	static value FASTCALL hook(FAST_ARGS);
	inline static call original;
};
_HOOK_END