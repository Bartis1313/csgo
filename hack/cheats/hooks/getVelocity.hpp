#pragma once

#include "helpers/common.hpp"

#include <SDK/math/Vector.hpp>

_HOOK_BEGIN
class GetVelocity
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, Vec3* a1, Vec3* a2);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, Vec3* a1, Vec3* a2);
	inline static call original;
};
_HOOK_END