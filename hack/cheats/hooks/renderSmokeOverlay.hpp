#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class RenderSmokeOverlay
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, bool);

	static value FASTCALL hook(FAST_ARGS, bool enabled);
	inline static call original;
};
_HOOK_END