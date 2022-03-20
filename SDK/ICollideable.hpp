#pragma once
#include "math/Vector.hpp"
#include "../utilities/vfunc.hpp"

class ICollideable
{
public:
	VFUNC(Vector&, OBBMins, 1, (), (this));
	VFUNC(Vector&, OBBMaxs, 2, (), (this));
	VFUNC(int, getSolid, 11, (), (this));
};