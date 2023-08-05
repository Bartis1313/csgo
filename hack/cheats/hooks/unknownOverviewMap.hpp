#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class UnknownOverviewMap
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int unk);
	inline static call original;
};
_HOOK_END