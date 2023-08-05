#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class GetPhysicsProperties
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int, float*, float*, float*, float*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int index, float* density, float* thickness, float* friction, float* elasticity);
	inline static call original;
};
_HOOK_END