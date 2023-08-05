#pragma once

#include "helpers/common.hpp"

#include <SDK/IVModelInfo.hpp>

_HOOK_BEGIN
class MapLoaderInit
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, model_t*, const char*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, model_t* model, const char* pathName);
	inline static call original;
};
_HOOK_END