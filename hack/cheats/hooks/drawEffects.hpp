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
	using call = value(THISCALL*)(THISPTR, double);

	static value FASTCALL hook(FAST_ARGS, double framerate);
	inline static call original;
};
_HOOK_END