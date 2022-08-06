#pragma once

#include "../base.hpp"

#include <vector>

class Screen2DEffectsType : public BaseHack
{
public:
	virtual void init() {};
	virtual void run() {};
	static void runAll();
protected:
	inline static std::vector<Screen2DEffectsType*> m_hacksRun;
};