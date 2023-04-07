#include "hooks.hpp"

#include <config/vars.hpp>
#include <gamememory/memory.hpp>

#include <intrin.h>

hooks::allocKeyValues::value FASTCALL hooks::allocKeyValues::hooked(FAST_ARGS, size_t size)
{
	if (const auto retAddr = reinterpret_cast<retaddr_t>(_ReturnAddress());
		retAddr == memory::allocKeyValuesEngine() || retAddr == memory::allocKeyValuesClient())
	{
		return nullptr;
	}

	return original(thisptr, size);
}

hooks::isUsingStaticPropDebugModes::value hooks::isUsingStaticPropDebugModes::hooked()
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

hooks::filesCheck::value FASTCALL hooks::filesCheck::hooked(FAST_ARGS)
{
	return 1; // 2 is kick
}

hooks::sv_cheats::value FASTCALL hooks::sv_cheats::hooked(FAST_ARGS)
{
	if (_ReturnAddress() == memory::camThink() && vars::misc->thirdp->enabled)
		return true;

	return original(thisptr);
}