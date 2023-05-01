#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class IsUsingStaticPropDebugModes
{
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(STDCALL*)();

	static value STDCALL hook();
	inline static call original;
};
_HOOK_END