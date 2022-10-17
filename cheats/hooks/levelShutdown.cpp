#include "hooks.hpp"

#include <SDK/interfaces/interfaces.hpp>

void __fastcall hooks::levelShutdown::hooked(FAST_ARGS)
{
	original(thisptr);
}