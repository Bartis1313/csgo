#include "hooks.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::isFollowingEntity::value FASTCALL hooks::isFollowingEntity::hooked(FAST_ARGS)
{
	if (vars::misc->disableItems->interpolate && _ReturnAddress() == memory::retAddrToInterpolation.cast<void*>()())
	{
		return true;
	}

	return original(thisptr);
}