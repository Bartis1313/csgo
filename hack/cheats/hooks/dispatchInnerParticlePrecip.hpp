#pragma once

#include "helpers/common.hpp"

#include <SDK/math/Vector.hpp>

_HOOK_BEGIN
class DispatchInnerParticlePrecip
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int, void*, Vec3);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int slot, void* player, Vec3 forward);
	inline static call original;
};
_HOOK_END