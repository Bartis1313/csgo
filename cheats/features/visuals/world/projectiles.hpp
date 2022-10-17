#pragma once

#include <classes/renderableToSurface.hpp>

class Projectiles : protected RenderableSurfaceType
{
public:
	constexpr Projectiles() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(Projectiles);
