#pragma once

#include "helpers/common.hpp"
#if UNLOCK_PRECIP_TESTING == true

#include <SDK/IVModelInfo.hpp>

_HOOK_BEGIN
class GetVCollide
{
	ADD_HOOK_FRIEND;
private:
	using value = vcollide_t*;
	using call = value(THISCALL*)(THISPTR, int);

	static value FASTCALL hook(FAST_ARGS, int modelIndex);
	inline static call original;
};
_HOOK_END
#endif