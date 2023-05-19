#include "isFollowingEntity.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::IsFollowingEntity::value hooks::IsFollowingEntity::hook(FAST_ARGS)
{
	if (vars::misc->disableItems->interpolate && reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory::retAddrToInterpolation())
	{
		return true;
	}

	return original(thisptr);
}