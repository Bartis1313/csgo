#pragma once

#include "../../classes/renderableToSurface.hpp"

class AimbotDraw : public RenderableSurfaceType
{
public:
	AimbotDraw()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
private:
	void drawFov();
	void drawBestPoint();
};

[[maybe_unused]] inline auto g_AimbotDraw = AimbotDraw{};