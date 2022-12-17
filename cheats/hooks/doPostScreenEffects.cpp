#include "hooks.hpp"

#include "../classes/doPostScreen.hpp"

#include <gamememory/memory.hpp>

int FASTCALL hooks::doPostScreenEffects::hooked(FAST_ARGS, int val)
{
	DoPostScreenType::runAll();

	return original(memory::interfaces::clientMode(), val);
}