#pragma once

#include <cheats/classes/renderableToSurface.hpp>

class DroppedWeapons : protected RenderableSurfaceType
{
public:
	constexpr DroppedWeapons() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(DroppedWeapons);
