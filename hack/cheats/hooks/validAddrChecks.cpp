#include "hooks.hpp"

#include <config/vars.hpp>
#include <gamememory/memory.hpp>

#include <intrin.h>

hooks::clientValidAddr::value FASTCALL hooks::clientValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

hooks::engineValidAddr::value FASTCALL hooks::engineValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

hooks::studioRenderValidAddr::value FASTCALL hooks::studioRenderValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

hooks::materialSystemValidAddr::value FASTCALL hooks::materialSystemValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

hooks::isUsingStaticPropDebugModes::value FASTCALL hooks::isUsingStaticPropDebugModes::hooked(FAST_ARGS)
{
	return vars::visuals->world->modulate->enabled; // might check ret addr, but pretty useless imho
}

hooks::doExtraBonesProcessing::value FASTCALL hooks::doExtraBonesProcessing::hooked(FAST_ARGS,
	[[maybe_unused]] void* hdr, [[maybe_unused]] void* pos,
	[[maybe_unused]] void* q, [[maybe_unused]] const Matrix3x4& matrix,
	[[maybe_unused]] void* computed, [[maybe_unused]] void* contex)
{
	return;
}

hooks::getUnverifiedFileHashes::value FASTCALL hooks::getUnverifiedFileHashes::hooked(FAST_ARGS, [[maybe_unused]] int maxFiles)
{
	return 0;
}

hooks::unkFileCheck::value FASTCALL hooks::unkFileCheck::hooked(FAST_ARGS)
{
	return 1; // 2 is kick
}

hooks::sv_cheats::value FASTCALL hooks::sv_cheats::hooked(FAST_ARGS)
{
	if (_ReturnAddress() == memory::camThink() && vars::misc->thirdp->enabled)
		return true;

	return original(thisptr);
}