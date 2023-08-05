#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class OverrideConfig
{
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, void*, bool);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, void* config, bool update);
	inline static call original;
};
_HOOK_END