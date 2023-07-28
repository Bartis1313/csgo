#pragma once

#include "helpers/common.hpp"

#include <SDK/math/Vector.hpp>

_HOOK_BEGIN
class DispatchInnerParticlePrecip
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, int, void*, Vec3);

	static value FASTCALL hook(FAST_ARGS, int slot, void* player, Vec3 forward);
	inline static call original;
};
_HOOK_END