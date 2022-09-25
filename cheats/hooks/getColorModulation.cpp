#include "hooks.hpp"

#include "../features/visuals/world/modulateColors.hpp"

#include <game/globals.hpp>

void __fastcall hooks::getColorModulation::hooked(FAST_ARGS, float* r, float* g, float* b)
{
	original(thisptr, r, g, b);

	g_ModulateColor.run(thisptr, r, g, b);
}