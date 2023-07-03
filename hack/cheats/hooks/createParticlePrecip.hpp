#pragma once

#include "helpers/common.hpp"
#if UNLOCK_PRECIP_TESTING == true

#include <SDK/EHandle.hpp>

_HOOK_BEGIN
class CreateParticlePrecip
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR);

	static value FASTCALL hook(FAST_ARGS);
	inline static call original;
};
_HOOK_END

#endif