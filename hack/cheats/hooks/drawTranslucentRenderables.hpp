#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class DrawTranslucentRenderables
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, bool, bool);

	static value FASTCALL hook(FAST_ARGS, bool inSkybox, bool shadowDepth);
	inline static call original;
};
_HOOK_END
