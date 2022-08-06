#pragma once

#include "../../../classes/doPostScreen.hpp"

class Glow : public DoPostScreenType
{
public:
	Glow()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run();
};

[[maybe_unused]] inline auto g_Glow = Glow{};