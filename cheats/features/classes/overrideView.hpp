#pragma once

#include "../base.hpp"

#include <vector>

class CViewSetup;

class OverrideViewType : public BaseHack
{
public:
	virtual void init() {};
	virtual void run(CViewSetup* view) {};
	static void runAll(CViewSetup* view);
protected:
	inline static std::vector<OverrideViewType*> m_hacksRun;
};