#include "hooks.hpp"

#include "../classes/doPostScreen.hpp"

#include <SDK/interfaces/interfaces.hpp>

int __stdcall hooks::doPostScreenEffects::hooked(int val)
{
	DoPostScreenType::runAll();

	return original(interfaces::clientMode, val);
}