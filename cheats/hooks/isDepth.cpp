#include "hooks.hpp"

#include "../features/sources/visuals/misc/motionblur.hpp"

bool __stdcall hooks::isDepthOfField::hooked()
{
	g_MotionBlur.render();

	return false;
}