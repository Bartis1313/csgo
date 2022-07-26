#include "hooks.hpp"

#include "../features/visuals/world.hpp"

bool __stdcall hooks::isDepthOfField::hooked()
{
	world.renderMotionBlur();

	return false;
}