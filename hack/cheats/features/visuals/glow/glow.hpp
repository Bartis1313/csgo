#pragma once

#include <SDK/math/Vector.hpp>
#include <render/Color.hpp>

#include "../streamproof/streamproof.hpp"

#define USE_GLOWBOX_CUSTOM true
// not recommended
#define USE_GLOW_CUSTOM true

namespace glow
{
	inline StreamProof streamProof{ "yologlow" };

	void run();
	void initMaterials();
	void addGlowBox(const Vec3& origin, const Vec3& angOrientation, const Vec3& mins, const Vec3& maxs, const Color& color, float lifetime);
}