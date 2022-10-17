#pragma once

#include <classes/renderableToSurface.hpp>

#include <SDK/math/Vector.hpp>

class Hat : protected RenderableSurfaceType
{
public:
	constexpr Hat() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
private:
	void drawConeEditedRainbow(const Vec3& pos, const float radius, const int points, const float size,
		const float speed, const int trianglesAlpha, const int linesAlpha, const float thickness = 2.0f);
};

GLOBAL_FEATURE(Hat);
