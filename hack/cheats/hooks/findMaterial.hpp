#pragma once

#include "helpers/common.hpp"
#include <SDK/material.hpp>

// char const* name, const char* group, bool complain = true, const char* prefix = nullptr

_HOOK_BEGIN
class FindMaterial
{
	ADD_HOOK_FRIEND;
private:
	using value = IMaterial*;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, char const* name, const char* group, bool complain, const char* prefix);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, char const* name, const char* group, bool complain, const char* prefix);
	inline static call original;
};
_HOOK_END