#pragma once

#include <classes/renderableToSurface.hpp>

class AimbotDraw : public RenderableSurfaceType
{
public:
	constexpr AimbotDraw() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	void drawFov();
	void drawBestPoint();
};

[[maybe_unused]] inline auto g_AimbotDraw = AimbotDraw{};