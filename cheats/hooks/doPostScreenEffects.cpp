#include "hooks.hpp"

#include "../classes/doPostScreen.hpp"

#include <SDK/interfaces/interfaces.hpp>

int __fastcall hooks::doPostScreenEffects::hooked(FAST_ARGS, int val)
{
	DoPostScreenType::runAll();

	return original(interfaces::clientMode, val);
}