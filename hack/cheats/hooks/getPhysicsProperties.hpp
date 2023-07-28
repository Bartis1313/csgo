#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class GetPhysicsProperties
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, int, float*, float*, float*, float*);

	static value FASTCALL hook(FAST_ARGS, int index, float* density, float* thickness, float* friction, float* elasticity);
	inline static call original;
};
_HOOK_END