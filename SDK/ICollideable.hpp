#pragma once

#include "math/Vector.hpp"
#include "math/matrix.hpp"

#include <utilities/vfunc.hpp>

class ICollideable
{
public:
	VFUNC(Vec3&, OBBMins, 1, (), (this));
	VFUNC(Vec3&, OBBMaxs, 2, (), (this));
	VFUNC(Vec3&, getCollisionAngles, 9, (), (this));
	VFUNC(Matrix3x4&, collisionToWorldTransform, 10, (), (this));
	VFUNC(int, getSolid, 11, (), (this));
	VFUNC(int, getSolidFlags, 12, (), (this));
};