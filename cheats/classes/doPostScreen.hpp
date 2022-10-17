#pragma once

#include "base.hpp"

#include <vector>

class DoPostScreenType : protected BaseHack
{
public:
	constexpr DoPostScreenType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}
public:
	static void runAll();
protected:
	virtual void run() = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<DoPostScreenType*> m_hacksRun;
};