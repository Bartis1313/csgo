#include "svcheats.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

#include <intrin.h>

hooks::SvCheats::value hooks::SvCheats::hook(HACK_FAST_ARGS)
{
	if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory::camThink() && vars::misc->thirdp->enabled)
		return true;

	return original(thisptr);
}