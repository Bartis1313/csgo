#pragma once

#include "base.hpp"

#include <vector>

class CViewSetup;

class OverrideViewType : protected BaseHack
{
public:
	constexpr OverrideViewType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(CViewSetup* view);
protected:
	virtual void run(CViewSetup* view) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void load() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
	virtual void roundRestart() override {};
private:
	inline static std::vector<OverrideViewType*> m_hacksRun;
};