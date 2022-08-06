#pragma once

#include "../base.hpp"

#include <vector>

class RenderablePresentType : public BaseHack
{
public:
	virtual void init() {};
	virtual void draw() {};
	static void runAll();
protected:
	inline static std::vector<RenderablePresentType*> m_hacksRun;
};