#pragma once

#include <cheats/classes/renderableToSurface.hpp>

class AimbotDraw : protected RenderableSurfaceType
{
public:
	constexpr AimbotDraw() :
		RenderableSurfaceType{}
	{}

	virtual void draw() override;
private:
	void drawFov();
	void drawBestPoint();
};

GLOBAL_FEATURE(AimbotDraw);