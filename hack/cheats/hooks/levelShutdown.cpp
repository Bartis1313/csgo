#include "hooks.hpp"

#include <SDK/interfaces/interfaces.hpp>

hooks::levelShutdown::value FASTCALL hooks::levelShutdown::hooked(FAST_ARGS)
{
	original(thisptr);
}