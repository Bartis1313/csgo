#pragma once

#include "helpers/common.hpp"

#include <SDK/math/matrix.hpp>
#include <SDK/math/vector.hpp>
#include <SDK/CStudioHdr.hpp>

_HOOK_BEGIN
class BuildTransformations
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, CStudioHdr*, void*, void*, const Matrix3x4&, int, void*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, CStudioHdr* hdr, void* pos, void* q, const Matrix3x4& matrix, int boneMask, void* computed);
	inline static call original;
};
_HOOK_END