#include "getVelocity.hpp"

hooks::GetVelocity::value hooks::GetVelocity::hook(FAST_ARGS, Vec3* a1, Vec3* a2)
{
	printf("called\n");
	return original(thisptr, a1, a2);
}