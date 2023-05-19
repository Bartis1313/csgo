#include "svcheats.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::SvCheats::value hooks::SvCheats::hook(FAST_ARGS)
{
	if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory::camThink() && vars::misc->thirdp->enabled)
		return true;

	return original(thisptr);
}