#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class GetUnverifiedFileHashes
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(THISCALL*)(THISPTR, int);

	static value FASTCALL hook(FAST_ARGS, int maxFiles);
	inline static call original;
};
_HOOK_END