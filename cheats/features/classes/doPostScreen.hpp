#pragma once

#include "../base.hpp"

#include <vector>

class DoPostScreenType : public BaseHack
{
public:
	virtual void init() {};
	virtual void run() {};
	static void runAll();
protected:
	inline static std::vector<DoPostScreenType*> m_hacksRun;
};