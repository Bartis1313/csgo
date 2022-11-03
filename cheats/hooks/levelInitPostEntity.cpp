#include "hooks.hpp"

#include <classes/base.hpp>

void __fastcall hooks::levelInitPostEntity::hooked(FAST_ARGS)
{
	BaseHack::loadAll();

	original(thisptr);
}