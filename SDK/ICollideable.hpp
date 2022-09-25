#pragma once

#include "math/Vector.hpp"
#include "math/matrix.hpp"

#include <utilities/vfunc.hpp>

class ICollideable
{
public:
	VFUNC(Vector&, OBBMins, 1, (), (this));
	VFUNC(Vector&, OBBMaxs, 2, (), (this));
	VFUNC(Vector&, getCollisionAngles, 9, (), (this));
	VFUNC(Matrix3x4&, collisionToWorldTransform, 10, (), (this));
	VFUNC(int, getSolid, 11, (), (this));
	VFUNC(int, getSolidFlags, 12, (), (this));
};