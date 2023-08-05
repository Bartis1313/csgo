#pragma once

#include "helpers/common.hpp"

#include <SDK/EHandle.hpp>

_HOOK_BEGIN
class RemoveEntity
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, void*, CBaseHandle);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, void* handleEnt, CBaseHandle handle);
	inline static call original;
};
_HOOK_END