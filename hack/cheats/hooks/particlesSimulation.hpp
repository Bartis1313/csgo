#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class ParticlesSimulations
{
public:
	static auto getOriginal() { return original; }
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR);

	static value FASTCALL hook(FAST_ARGS);
	inline static call original;
};
_HOOK_END