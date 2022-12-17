#include "hooks.hpp"

#include "../features/visuals/misc/motionblur.hpp"

bool FASTCALL hooks::isDepthOfField::hooked(FAST_ARGS)
{
	MotionBlur::render();

	return false;
}