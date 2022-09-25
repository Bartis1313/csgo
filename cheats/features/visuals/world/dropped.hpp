#pragma once

#include <classes/renderableToSurface.hpp>

class DroppedWeapons : public RenderableSurfaceType
{
public:
	constexpr DroppedWeapons() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_DroppedWeapons = DroppedWeapons{};
