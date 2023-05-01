#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class AllocKeyValues
{
	ADD_HOOK_FRIEND;
private:
	using value = void*;
	using call = value(THISCALL*)(THISPTR, size_t);

	static value FASTCALL hook(FAST_ARGS, size_t size);
	inline static call original;
};
_HOOK_END