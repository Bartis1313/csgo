#pragma once

#include "base.hpp"

#include <vector>

class DoPostScreenType : public BaseHack
{
public:
	constexpr DoPostScreenType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run() {};
	static void runAll();
protected:
	inline static std::vector<DoPostScreenType*> m_hacksRun;
};