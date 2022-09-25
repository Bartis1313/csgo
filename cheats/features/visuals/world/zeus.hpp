#pragma once

#include <classes/renderableToSurface.hpp>

class IConVar;

class ZeusDraw : public RenderableSurfaceType
{
public:
	constexpr ZeusDraw() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	IConVar* m_party;
};

[[maybe_unused]] inline auto g_ZeusDraw = ZeusDraw{};
