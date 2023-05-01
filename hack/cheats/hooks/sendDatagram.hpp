#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class SendDatagram
{
public:
	static auto getOriginal() { return original; }
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(THISCALL*)(THISPTR, void*);

	static value FASTCALL hook(FAST_ARGS, void* datagram);
	inline static call original;
};
_HOOK_END