#include "getColorModulation.hpp"

#include <cheats/features/visuals/world/modulateColors.hpp>

hooks::GetColorModulation::value hooks::GetColorModulation::hook(HACK_FAST_ARGS, float* r, float* g, float* b)
{
	original(thisptr, r, g, b);

	modulateColor::run(thisptr, r, g, b);
}