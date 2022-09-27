#include "hooks.hpp"

#include <config/vars.hpp>
#include <gamememory/memory.hpp>

#include <intrin.h>

char __fastcall hooks::clientValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::engineValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::studioRenderValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::materialSystemValidAddr::hooked(FAST_ARGS, [[maybe_unused]] const char* lpModuleName)
{
	return 1;
}

bool __fastcall hooks::isUsingStaticPropDebugModes::hooked(FAST_ARGS)
{
	return config.get<bool>(vars.bModulateColor); // might check ret addr, but pretty useless imho
}

void __fastcall hooks::doExtraBonesProcessing::hooked(FAST_ARGS,
	[[maybe_unused]] void* hdr, [[maybe_unused]] void* pos,
	[[maybe_unused]] void* q, [[maybe_unused]] const Matrix3x4& matrix,
	[[maybe_unused]] void* computed, [[maybe_unused]] void* contex)
{
	return;
}

int __fastcall hooks::getUnverifiedFileHashes::hooked(FAST_ARGS, [[maybe_unused]] int maxFiles)
{
	return 0;
}

int __fastcall hooks::unkFileCheck::hooked(FAST_ARGS)
{
	return 1; // 2 is kick
}


bool __fastcall hooks::sv_cheats::hooked(FAST_ARGS)
{
	if (_ReturnAddress() == g_Memory.m_camThink() && config.get<bool>(vars.bThirdp))
		return true;

	return original(thisptr);
}