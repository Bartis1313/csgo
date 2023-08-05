#pragma once

#include "helpers/common.hpp"

#include <SDK/material.hpp>

_HOOK_BEGIN
class ViewFade
{
public:
	static auto getOriginal() { return original; }
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, uint8_t*, IMaterial*, bool);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, uint8_t* color, IMaterial* material, bool t);
	inline static call original;
};
_HOOK_END