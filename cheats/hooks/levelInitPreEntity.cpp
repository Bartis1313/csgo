#include "hooks.hpp"

#include <classes/base.hpp>

void __fastcall hooks::levelInitPreEntity::hooked(FAST_ARGS, const char* map)
{
	BaseHack::resetAll();

	original(thisptr, map);
}