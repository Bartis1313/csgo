#include "hooks.hpp"

#include "../features/visuals/world/modulateColors.hpp"

#include <game/globals.hpp>

void FASTCALL hooks::getColorModulation::hooked(FAST_ARGS, float* r, float* g, float* b)
{
	original(thisptr, r, g, b);

	ModulateColor::run(thisptr, r, g, b);
}