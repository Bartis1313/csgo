#pragma once

#include "base.hpp"

#include <vector>

class CViewSetup;

class RenderViewType : protected BaseHack
{
public:
	constexpr RenderViewType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(const CViewSetup& view);
protected:
	virtual void run(const CViewSetup& view) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void load() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<RenderViewType*> m_hacksRun;
};