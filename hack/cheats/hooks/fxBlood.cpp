#include "hooks.hpp"

// why this doesn't work or gets called?
// these floats are treated like 0-255 color
hooks::fxBlood::value FASTCALL hooks::fxBlood::hooked(FAST_ARGS, Vec3& pos, Vec3& dir, float r, float g, float b, float a)
{
	return original(thisptr, pos, dir, 0, 100, 200, 255);
}