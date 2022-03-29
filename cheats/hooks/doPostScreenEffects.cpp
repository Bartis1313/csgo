#include "hooks.hpp"

#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/visuals/glow.hpp"
#include "../features/misc/misc.hpp"
#include "../features/visuals/world.hpp"
#include "../globals.hpp"

int __stdcall hooks::doPostScreenEffects::hooked(int val)
{
	glow.run();
	misc.thirdperson();
	world.removeSky(globals::isShutdown);

	return original(interfaces::clientMode, val);
}