#pragma once

#include "base.hpp"

#include <vector>

class CViewSetup;

class RenderViewType : public BaseHack
{
public:
	constexpr RenderViewType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(const CViewSetup& view) {};
	static void runAll(const CViewSetup& view);
protected:
	inline static std::vector<RenderViewType*> m_hacksRun;
};