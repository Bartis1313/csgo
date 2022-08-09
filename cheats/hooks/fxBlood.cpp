#include "hooks.hpp"

// why this doesn't work or gets called?
// these floats are treated like 0-255 color
void __fastcall hooks::fxBlood::hooked(FAST_ARGS, Vector& pos, Vector& dir, float r, float g, float b, float a)
{
	return original(thisptr, pos, dir, 0, 100, 200, 255);
}