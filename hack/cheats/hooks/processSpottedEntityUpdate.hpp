#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class ProcessSpottedEntityUpdate
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(THISCALL*)(THISPTR, void*);

	static value FASTCALL hook(FAST_ARGS, void* unk);
	inline static call original;
};
_HOOK_END