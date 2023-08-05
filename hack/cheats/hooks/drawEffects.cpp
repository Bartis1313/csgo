#include "drawEffects.hpp"

hooks::DrawEffects::value hooks::DrawEffects::hook(HACK_FAST_ARGS, double framerate)
{
	original(thisptr, framerate);
}