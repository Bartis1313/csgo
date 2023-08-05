#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class ProcessSpottedEntityUpdate
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, void*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, void* unk);
	inline static call original;
};
_HOOK_END