#include "drawEffects.hpp"

hooks::DrawEffects::value hooks::DrawEffects::hook(FAST_ARGS, double framerate)
{
	original(thisptr, framerate);
}