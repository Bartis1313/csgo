#include "hooks.hpp"

#include "../features/visuals/world/modulateColors.hpp"

#include <cheats/game/globals.hpp>

hooks::getColorModulation::value hooks::getColorModulation::hooked(FAST_ARGS, float* r, float* g, float* b)
{
	original(thisptr, r, g, b);

	ModulateColor::run(thisptr, r, g, b);
}