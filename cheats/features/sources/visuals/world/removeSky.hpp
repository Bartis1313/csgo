#pragma once

#include "../../../classes/doPostScreen.hpp"

class IConVar;

class RemoveSky : public DoPostScreenType
{
public:
	RemoveSky()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run();
private:
	IConVar* m_sky;
};

[[maybe_unused]] inline auto g_RemoveSky = RemoveSky{};
