#include "hooks.hpp"

#include <classes/base.hpp>
#include <gamememory/memory.hpp>

void FASTCALL hooks::levelInitPostEntity::hooked(FAST_ARGS)
{
	g_Memory.postInit();
	BaseHack::loadAll();

	original(thisptr);
}