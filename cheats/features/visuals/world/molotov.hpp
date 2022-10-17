#pragma once

#include <classes/renderableToSurface.hpp>

class Inferno_t;

class MolotovDraw : protected RenderableSurfaceType
{
public:
	constexpr MolotovDraw() : 
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(MolotovDraw);
