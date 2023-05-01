#include "isFollowingEntity.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::IsFollowingEntity::value hooks::IsFollowingEntity::hook(FAST_ARGS)
{
	if (vars::misc->disableItems->interpolate && memory::retAddr() == memory::retAddrToInterpolation())
	{
		return true;
	}

	return original(thisptr);
}