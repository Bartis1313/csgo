#include "svcheats.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::SvCheats::value hooks::SvCheats::hook(FAST_ARGS)
{
	if (memory::retAddr() == memory::camThink() && vars::misc->thirdp->enabled)
		return true;

	return original(thisptr);
}