#include "CGlowManager.hpp"

#include <gamememory/memory.hpp>

void CGlowManager::addGlowBox(const Vec3& origin, const Vec3& angle, const Vec3& min, const Vec3& max, const Color& color, float lifetime)
{
	memory::addGlowBox()(this, origin, angle, min, max, SDKColor{ color }, lifetime);
}