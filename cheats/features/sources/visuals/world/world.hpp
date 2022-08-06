#pragma once

#include "../../../classes/renderableToSurface.hpp"

class World : public RenderableSurfaceType
{
public:
	constexpr World() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_World = World{};
