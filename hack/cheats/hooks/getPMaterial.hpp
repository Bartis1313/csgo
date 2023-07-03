#pragma once

#include "helpers/common.hpp"

#include <SDK/EHandle.hpp>

_HOOK_BEGIN
class GetPMaterial
{
	ADD_HOOK_FRIEND;
private:
	using value = void*;
	using call = value(THISCALL*)(THISPTR, const char*);

	static value FASTCALL hook(FAST_ARGS, const char* name);
	inline static call original;
};
_HOOK_END