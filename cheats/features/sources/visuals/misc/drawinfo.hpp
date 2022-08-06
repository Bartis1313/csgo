#pragma once

#include "../../../classes/renderableToPresent.hpp"

class Info : public RenderablePresentType
{
public:
	Info()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}	

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_Information = Info{};
