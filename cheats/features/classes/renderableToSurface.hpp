#pragma once

#include "../base.hpp"

#include <vector>

class RenderableSurfaceType : public BaseHack
{
public:
	virtual void init() {};
	virtual void draw() {};
	static void runAll();
protected:
	inline static std::vector<RenderableSurfaceType*> m_hacksRun;
};