#pragma once
#include "math/Vector.hpp"
#include "../utilities/vfunc.hpp"

class IVDebugOverlay
{
public:
	VFUNC(bool, screenPosition, 11, (const Vector& in, Vector& out), (this, std::ref(in), std::ref(out)));
	VFUNC(bool, worldToScreen, 13, (const Vector& in, Vector& out), (this, std::ref(in), std::ref(out)));
};