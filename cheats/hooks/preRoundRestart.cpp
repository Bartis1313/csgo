#include "hooks.hpp"

hooks::preRestartRound::value FASTCALL hooks::preRestartRound::hooked(FAST_ARGS)
{
	original(thisptr);
}