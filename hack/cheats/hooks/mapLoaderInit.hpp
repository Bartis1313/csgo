#pragma once

#include "helpers/common.hpp"

#include <SDK/IVModelInfo.hpp>

_HOOK_BEGIN
class MapLoaderInit
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, model_t*, const char*);

	static value FASTCALL hook(FAST_ARGS, model_t* model, const char* pathName);
	inline static call original;
};
_HOOK_END