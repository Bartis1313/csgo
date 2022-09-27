#pragma once

#include <classes/renderableToSurface.hpp>

class IConVar;

class Crosshair : public RenderableSurfaceType
{
public:
	constexpr Crosshair() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	IConVar* m_scale;
	IConVar* m_crosshairRecoil;
};

[[maybe_unused]] inline auto g_Crosshair = Crosshair{};
