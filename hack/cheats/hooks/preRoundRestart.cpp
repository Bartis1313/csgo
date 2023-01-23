#include "hooks.hpp"

#include <cheats/classes/base.hpp>

hooks::preRestartRound::value FASTCALL hooks::preRestartRound::hooked(FAST_ARGS)
{
	BaseHack::restartRoundAll();

	original(thisptr);
}