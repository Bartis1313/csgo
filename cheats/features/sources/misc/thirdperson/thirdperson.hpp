#pragma once

#include "../../../classes/doPostScreen.hpp"

class Thirdperson : public DoPostScreenType
{
public:
	Thirdperson()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run();
};

[[maybe_unused]] inline auto g_Thirdperson = Thirdperson{};
