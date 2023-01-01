#include "hooks.hpp"

#include <classes/base.hpp>

hooks::levelInitPreEntity::value FASTCALL hooks::levelInitPreEntity::hooked(FAST_ARGS, const char* map)
{
	BaseHack::resetAll();

	original(thisptr, map);
}