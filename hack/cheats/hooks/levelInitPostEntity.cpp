#include "hooks.hpp"

#include <cheats/classes/base.hpp>
#include <gamememory/memory.hpp>

hooks::levelInitPostEntity::value FASTCALL hooks::levelInitPostEntity::hooked(FAST_ARGS)
{
	memory::postInit();
	BaseHack::loadAll();

	original(thisptr);
}