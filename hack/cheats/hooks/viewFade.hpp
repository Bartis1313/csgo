#pragma once

#include "helpers/common.hpp"

#include <SDK/material.hpp>

_HOOK_BEGIN
class ViewFade
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, uint8_t*, IMaterial*, bool);

	static value FASTCALL hook(FAST_ARGS, uint8_t* color, IMaterial* material, bool t);
	inline static call original;
};
_HOOK_END