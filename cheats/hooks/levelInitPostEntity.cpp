#include "hooks.hpp"

#include <SDK/interfaces/interfaces.hpp>

void __fastcall hooks::levelInitPostEntity::hooked(FAST_ARGS)
{
	original(thisptr);
}