#include "hooks.hpp"

#include "../features/visuals/world.hpp"
#include "../globals.hpp"

void __fastcall hooks::getColorModulation::hooked(FAST_ARGS, float* r, float* g, float* b)
{
	original(thisptr, r, g, b);

	world.modulateWorld(thisptr, r, g, b, globals::isShutdown);
}