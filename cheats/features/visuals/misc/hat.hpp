#pragma once

#include <classes/renderableToSurface.hpp>

#include <SDK/math/Vector.hpp>

class Hat : public RenderableSurfaceType
{
public:
	constexpr Hat() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	void drawConeEditedRainbow(const Vec3& pos, const float radius, const int points, const float size,
		const float speed, const int trianglesAlpha, const int linesAlpha, const float thickness = 2.0f);
};

[[maybe_unused]] inline auto g_Hat = Hat{};
