#pragma once

#include "helpers/common.hpp"

#include <SDK/CViewSetup.hpp>

_HOOK_BEGIN
class DrawWorldAndEntities
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, bool, const CViewSetup&, int, void*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, bool drawSkybox, const CViewSetup& viewIn, int clearFlags, void* customVisibility);
	inline static call original;
};
_HOOK_END