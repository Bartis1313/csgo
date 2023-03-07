#include "hooks.hpp"

#include <cheats/classes/overrideM.hpp>

hooks::overrideMouse::value FASTCALL hooks::overrideMouse::hooked(FAST_ARGS, float* x, float* y)
{
	original(thisptr, x, y);
	OverrideMouseType::runAll(x, y);
}