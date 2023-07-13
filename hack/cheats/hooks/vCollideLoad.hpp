#pragma once

#include "helpers/common.hpp"

#include <SDK/IVModelInfo.hpp>

_HOOK_BEGIN
class VCollideLoad
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, vcollide_t*, int, const char*, int, bool);

	static value FASTCALL hook(FAST_ARGS, vcollide_t* pOutput, int solidCount, const char* pBuffer, int size, bool swap);
	inline static call original;
};
_HOOK_END