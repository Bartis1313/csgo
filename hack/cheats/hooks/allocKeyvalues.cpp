#include "allocKeyvalues.hpp"

#include <gamememory/memory.hpp>

#include <intrin.h>

hooks::AllocKeyValues::value hooks::AllocKeyValues::hook(FAST_ARGS, size_t size)
{
	if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory::allocKeyValuesEngine()
		|| reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory::allocKeyValuesClient())
	{
		return nullptr;
	}

	return original(thisptr, size);
}
