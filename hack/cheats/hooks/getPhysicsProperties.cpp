#include "getPhysicsProperties.hpp"

hooks::GetPhysicsProperties::value hooks::GetPhysicsProperties::hook(HACK_FAST_ARGS, int index, float* density, float* thickness, float* friction, float* elasticity)
{
	original(thisptr, index, density, thickness, friction, elasticity);

	printf("for model index %i\n", index);
}