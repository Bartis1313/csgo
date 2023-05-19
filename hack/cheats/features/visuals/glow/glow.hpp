#pragma once

#include <SDK/math/Vector.hpp>
#include <render/Color.hpp>

class Entity_t;

namespace glow
{
	void run();
	void initMaterials();
	void addGlowBox(const Vec3& origin, const Vec3& angOrientation, const Vec3& mins, const Vec3& maxs, const Color& color, float lifetime);
}