#pragma once

#include "helpers/common.hpp"

#include <SDK/math/Vector.hpp>

_HOOK_BEGIN
class NewParticleEffectSetControlPoint
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, int, const Vec3&);

	static value FASTCALL hook(FAST_ARGS, int whichPoint, const Vec3& vec);
	inline static call original;
};
_HOOK_END