#include "hooks.hpp"

#include <SDK/interfaces/interfaces.hpp>

void FASTCALL hooks::levelShutdown::hooked(FAST_ARGS)
{
	original(thisptr);
}