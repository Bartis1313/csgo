#pragma once

#include "base.hpp"

class OverrideMouseType : protected BaseHack
{
public:
	constexpr OverrideMouseType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(float* x, float* y);
protected:
	virtual void run(float* x, float* y) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void load() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
	virtual void roundRestart() override {};
private:
	inline static std::vector<OverrideMouseType*> m_hacksRun;
};