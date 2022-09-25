#pragma once

#include "base.hpp"

#include <vector>

class RenderableSurfaceType : public BaseHack
{
public:
	constexpr RenderableSurfaceType()
		: BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void draw() {};
	static void runAll();
protected:
	inline static std::vector<RenderableSurfaceType*> m_hacksRun;
};