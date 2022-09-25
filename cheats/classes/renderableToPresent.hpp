#pragma once

#include "base.hpp"

#include <vector>

class RenderablePresentType : public BaseHack
{
public:
	constexpr RenderablePresentType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void draw() {};
	static void runAll();
protected:
	inline static std::vector<RenderablePresentType*> m_hacksRun;
};