#include "hooks.hpp"
#include "../features/visuals/glow.hpp"
#include "../features/misc/misc.hpp"

int __stdcall hooks::doPostScreenEffects::hooked(int val)
{
	glow::run();
	misc::thirdperson();

	return original(interfaces::clientMode, val);
}