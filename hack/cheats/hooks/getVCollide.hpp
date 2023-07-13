#pragma once

#include "helpers/common.hpp"

#include <SDK/IVModelInfo.hpp>

_HOOK_BEGIN
class GetVCollide
{
	ADD_HOOK_FRIEND;
private:
	using value = vcollide_t*;
	using call = value(THISCALL*)(THISPTR, int);

	static value FASTCALL hook(FAST_ARGS, int modelIndex);
	inline static call original;
};
_HOOK_END