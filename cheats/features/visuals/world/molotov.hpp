#pragma once

#include <classes/renderableToSurface.hpp>

class Inferno_t;

class MolotovDraw : public RenderableSurfaceType
{
public:
	constexpr MolotovDraw() : 
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_Molotov = MolotovDraw{};
