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
	using call = value(THISCALL*)(THISPTR, CStudioHdr*, void*, void*, const Matrix3x4&, int, void*);

	static value FASTCALL hook(FAST_ARGS, CStudioHdr* hdr, void* pos, void* q, const Matrix3x4& matrix, int boneMask, void* computed);
	inline static call original;
};
_HOOK_END