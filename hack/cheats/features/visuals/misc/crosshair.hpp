#pragma once

#include <cheats/classes/renderableToSurface.hpp>

class IConVar;

class Crosshair : protected RenderableSurfaceType
{
public:
	constexpr Crosshair() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void init() override;
	virtual void draw() override;
	virtual void shutdown() override;
private:
	IConVar* m_crosshair;
	IConVar* m_scale;
	IConVar* m_crosshairRecoil;
};

GLOBAL_FEATURE(Crosshair);
