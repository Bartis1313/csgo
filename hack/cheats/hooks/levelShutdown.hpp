#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class LevelShudown
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS);
	inline static call original;
};
_HOOK_END