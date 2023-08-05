#pragma once

#pragma once

#include "helpers/common.hpp"

#include <SDK/math/matrix.hpp>

_HOOK_BEGIN
class DoExtraBonesProcessing
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, void*, void*, void*, const Matrix3x4&, void*, void*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, [[maybe_unused]] void* hdr, [[maybe_unused]] void* pos,
		[[maybe_unused]] void* q, [[maybe_unused]] const Matrix3x4& matrix,
		[[maybe_unused]] void* computed, [[maybe_unused]] void* contex);
	inline static call original;
};
_HOOK_END