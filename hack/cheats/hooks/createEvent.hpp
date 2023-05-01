#pragma once

#include "helpers/common.hpp"

#include <SDK/IGameEvent.hpp>

#undef CreateEvent

_HOOK_BEGIN
class CreateEvent
{
	ADD_HOOK_FRIEND;
private:
	using value = IGameEvent*;
	using call = value(THISCALL*)(THISPTR, const char*, bool, uint32_t);

	static value FASTCALL hook(FAST_ARGS, const char* name, bool allowed, uint32_t unk);
	inline static call original;
};
_HOOK_END