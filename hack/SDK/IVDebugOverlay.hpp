#pragma once

#include "math/Vector.hpp"

#include "helpers/vfunc.hpp"

class IVDebugOverlay
{
public:
	VFUNC(void, box, 2, (const Vec3& origin, const Vec3& mins, const Vec3& max, Vec3 const& orientation,
		int r, int g, int b, int a, float duration), (this, std::cref(origin), std::cref(mins), std::cref(max), std::cref(orientation), r, g, b, a, duration));
	VFUNC(bool, worldToScreen, 13, (const Vec3& in, Vec3& out), (this, std::cref(in), std::ref(out)));
};