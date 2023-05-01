#pragma once

#include "helpers/common.hpp"

#include <SDK/math/Vector.hpp>

_HOOK_BEGIN
class PlayStepSound
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(THISCALL*)(THISPTR, Vec3&, void*, float, bool, void*);

	static value FASTCALL hook(FAST_ARGS, Vec3& origin, void* surface, float volume, bool force, void* unk);
	inline static call original;
};
_HOOK_END