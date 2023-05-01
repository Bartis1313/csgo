#include "allocKeyvalues.hpp"

#include <gamememory/memory.hpp>

hooks::AllocKeyValues::value hooks::AllocKeyValues::hook(FAST_ARGS, size_t size)
{
	if (memory::retAddr() == memory::allocKeyValuesEngine() || memory::retAddr() == memory::allocKeyValuesClient())
	{
		return nullptr;
	}

	return original(thisptr, size);
}
