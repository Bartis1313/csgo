#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class AllocKeyValues
{
	ADD_HOOK_FRIEND;
private:
	using value = void*;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, size_t);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, size_t size);
	inline static call original;
};
_HOOK_END