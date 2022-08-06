#pragma once

#include "../../../classes/renderableToSurface.hpp"

class IConVar;

class Crosshair : public RenderableSurfaceType
{
public:
	Crosshair()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
private:
	IConVar* m_scale;
};

[[maybe_unused]] inline auto g_Crosshair = Crosshair{};
