#pragma once

#include "math/Vector.hpp"

#include "helpers/vfunc.hpp"

class IVDebugOverlay
{
public:
	VFUNC(bool, worldToScreen, 13, (const Vec3& in, Vec3& out), (this, std::cref(in), std::ref(out)));
};