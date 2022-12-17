#include "hooks.hpp"

#include <classes/base.hpp>

void FASTCALL hooks::levelInitPreEntity::hooked(FAST_ARGS, const char* map)
{
	BaseHack::resetAll();

	original(thisptr, map);
}