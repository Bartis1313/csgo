#pragma once

#include "helpers/common.hpp"

#include <SDK/math/Vector.hpp>

_HOOK_BEGIN
class GetVelocity
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, Vec3* a1, Vec3* a2);

	static value FASTCALL hook(FAST_ARGS, Vec3* a1, Vec3* a2);
	inline static call original;
};
_HOOK_END