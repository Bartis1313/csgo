#pragma once

#include <cheats/classes/renderableToSurface.hpp>

class IConVar;

class ZeusDraw : protected RenderableSurfaceType
{
public:
	constexpr ZeusDraw() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
	virtual void init() override;
private:
	IConVar* m_party;
};

GLOBAL_FEATURE(ZeusDraw);
