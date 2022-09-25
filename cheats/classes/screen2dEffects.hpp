#pragma once

#include "base.hpp"

#include <vector>

class Screen2DEffectsType : public BaseHack
{
public:
	constexpr Screen2DEffectsType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run() {};
	static void runAll();
protected:
	inline static std::vector<Screen2DEffectsType*> m_hacksRun;
};