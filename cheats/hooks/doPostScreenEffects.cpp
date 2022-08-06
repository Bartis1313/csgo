#include "hooks.hpp"

#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/classes/doPostScreen.hpp"

int __stdcall hooks::doPostScreenEffects::hooked(int val)
{
	DoPostScreenType::runAll();

	return original(interfaces::clientMode, val);
}