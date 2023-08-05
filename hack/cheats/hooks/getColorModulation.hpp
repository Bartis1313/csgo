#pragma once

#include "helpers/common.hpp"

#include <SDK/Enums.hpp>

_HOOK_BEGIN
class GetColorModulation
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, float*, float*, float*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, float* r, float* g, float* b);
	inline static call original;
};
_HOOK_END