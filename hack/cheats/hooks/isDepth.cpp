#include "isDepth.hpp"

#include <cheats/features/visuals/misc/motionblur.hpp>

hooks::IsDepthOfField::value hooks::IsDepthOfField::hook()
{
	motionBlur::render();

	return false;
}