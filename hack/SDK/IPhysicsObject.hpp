#pragma once

#include "helpers/vfunc.hpp"
#include "math/Vector.hpp"

class IPhysicsObject
{
public:
	VFUNC(void, getVelocity, 52, (Vec3* velocity, Vec3* angularVelocity), (this, velocity, angularVelocity));
};