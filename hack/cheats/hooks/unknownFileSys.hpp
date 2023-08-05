#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class UnknownFileSysAlloc
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, void*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, void* image);
	inline static call original;
};
_HOOK_END