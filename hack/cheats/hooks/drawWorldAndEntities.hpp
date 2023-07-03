#pragma once

#include "helpers/common.hpp"

#include <SDK/CViewSetup.hpp>

_HOOK_BEGIN
class DrawWorldAndEntities
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, bool, const CViewSetup&, int, void*);

	static value FASTCALL hook(FAST_ARGS, bool drawSkybox, const CViewSetup& viewIn, int clearFlags, void* customVisibility);
	inline static call original;
};
_HOOK_END