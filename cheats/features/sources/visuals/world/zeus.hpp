#pragma once

#include "../../../classes/renderableToSurface.hpp"

class IConVar;

class ZeusDraw : public RenderableSurfaceType
{
public:
	ZeusDraw()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
private:
	IConVar* m_party;
};

[[maybe_unused]] inline auto g_ZeusDraw = ZeusDraw{};
