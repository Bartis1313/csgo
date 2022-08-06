#pragma once

#include "../../../classes/renderableToSurface.hpp"

class World : public RenderableSurfaceType
{
public:
	World()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_World = World{};
