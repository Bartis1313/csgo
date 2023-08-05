#pragma once

#include "helpers/common.hpp"

#include <SDK/Enums.hpp>
#include <SDK/CViewSetup.hpp>

_HOOK_BEGIN
class DrawEffects
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, double);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, double framerate);
	inline static call original;
};
_HOOK_END