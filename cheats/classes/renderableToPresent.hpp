#pragma once

#include "base.hpp"

#include <vector>

class RenderablePresentType : protected BaseHack
{
public:
	constexpr RenderablePresentType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll();
protected:
	virtual void draw() = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<RenderablePresentType*> m_hacksRun;
};