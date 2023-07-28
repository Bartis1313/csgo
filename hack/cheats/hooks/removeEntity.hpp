#pragma once

#include "helpers/common.hpp"

#include <SDK/EHandle.hpp>

_HOOK_BEGIN
class RemoveEntity
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, void*, CBaseHandle);

	static value FASTCALL hook(FAST_ARGS, void* handleEnt, CBaseHandle handle);
	inline static call original;
};
_HOOK_END