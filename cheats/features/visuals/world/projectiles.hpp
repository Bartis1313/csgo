#pragma once

#include <classes/renderableToSurface.hpp>

class Projectiles : public RenderableSurfaceType
{
public:
	constexpr Projectiles() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_Projectiles = Projectiles{};
