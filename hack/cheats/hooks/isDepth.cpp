#include "hooks.hpp"

#include "../features/visuals/misc/motionblur.hpp"

hooks::isDepthOfField::value hooks::isDepthOfField::hooked()
{
	g_MotionBlur->render();

	return false;
}