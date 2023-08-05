#pragma once

#include "helpers/common.hpp"

#include <SDK/EHandle.hpp>

_HOOK_BEGIN
class GetPMaterial
{
	ADD_HOOK_FRIEND;
private:
	using value = void*;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, const char*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, const char* name);
	inline static call original;
};
_HOOK_END