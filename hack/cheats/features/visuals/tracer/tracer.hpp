#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>

class BulletTracer : protected RenderableSurfaceType
{
public:
	constexpr BulletTracer() :
		RenderableSurfaceType{}
	{}

protected:
	void draw() override;
};

GLOBAL_FEATURE(BulletTracer);
