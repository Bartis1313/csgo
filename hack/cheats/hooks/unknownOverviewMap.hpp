#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class UnknownOverviewMap
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(THISCALL*)(THISPTR, int);

	static value FASTCALL hook(FAST_ARGS, int unk);
	inline static call original;
};
_HOOK_END