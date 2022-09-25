#pragma once

#include "math/Vector.hpp"

#include <utilities/vfunc.hpp>

class IVDebugOverlay
{
public:
	VFUNC(bool, worldToScreen, 13, (const Vector& in, Vector& out), (this, std::cref(in), std::ref(out)));
};