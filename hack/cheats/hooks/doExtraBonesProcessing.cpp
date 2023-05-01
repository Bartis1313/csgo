#include "doExtraBonesProcessing.hpp"

hooks::DoExtraBonesProcessing::value hooks::DoExtraBonesProcessing::hook(FAST_ARGS, [[maybe_unused]] void* hdr, [[maybe_unused]] void* pos,
	[[maybe_unused]] void* q, [[maybe_unused]] const Matrix3x4& matrix,
	[[maybe_unused]] void* computed, [[maybe_unused]] void* contex)
{
	return;
}