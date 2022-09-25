#pragma once

#include "base.hpp"

#include <vector>

class CViewSetup;

class OverrideViewType : public BaseHack
{
public:
	constexpr OverrideViewType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(CViewSetup* view) {};
	static void runAll(CViewSetup* view);
protected:
	inline static std::vector<OverrideViewType*> m_hacksRun;
};