#include "hooks.hpp"

#include "../features/visuals/misc/motionblur.hpp"

hooks::isDepthOfField::value FASTCALL hooks::isDepthOfField::hooked(FAST_ARGS)
{
	MotionBlur::render();

	return false;
}