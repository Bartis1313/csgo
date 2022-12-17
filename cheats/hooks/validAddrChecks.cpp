#include "hooks.hpp"

#include <config/vars.hpp>
#include <gamememory/memory.hpp>

#include <intrin.h>

char FASTCALL hooks::clientValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

char FASTCALL hooks::engineValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

char FASTCALL hooks::studioRenderValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

char FASTCALL hooks::materialSystemValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

bool FASTCALL hooks::isUsingStaticPropDebugModes::hooked(FAST_ARGS)
{
	return vars::visuals->world->modulate->enabled; // might check ret addr, but pretty useless imho
}

void FASTCALL hooks::doExtraBonesProcessing::hooked(FAST_ARGS,
	[[maybe_unused]] void* hdr, [[maybe_unused]] void* pos,
	[[maybe_unused]] void* q, [[maybe_unused]] const Matrix3x4& matrix,
	[[maybe_unused]] void* computed, [[maybe_unused]] void* contex)
{
	return;
}

int FASTCALL hooks::getUnverifiedFileHashes::hooked(FAST_ARGS, [[maybe_unused]] int maxFiles)
{
	return 0;
}

int FASTCALL hooks::unkFileCheck::hooked(FAST_ARGS)
{
	return 1; // 2 is kick
}


bool FASTCALL hooks::sv_cheats::hooked(FAST_ARGS)
{
	if (_ReturnAddress() == memory::camThink() && vars::misc->thirdp->enabled)
		return true;

	return original(thisptr);
}