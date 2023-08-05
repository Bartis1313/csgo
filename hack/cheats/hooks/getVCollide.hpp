#pragma once

#include "helpers/common.hpp"

#include <SDK/IVModelInfo.hpp>

_HOOK_BEGIN
class GetVCollide
{
	ADD_HOOK_FRIEND;
private:
	using value = vcollide_t*;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int modelIndex);
	inline static call original;
};
_HOOK_END