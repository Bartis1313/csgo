#include "drawSetColor.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::setDrawColor::value hooks::setDrawColor::hook(FAST_ARGS, int r, int g, int b, int a)
{
	const auto ret = reinterpret_cast<uintptr_t>(_ReturnAddress());

	if (vars::misc->scope->enabled)
	{
		if (ret == memory::scopeArc() || ret == memory::scopeDust())
		{
			a = 0;
		}
	}

	return original(thisptr, r, g, b, a);
}