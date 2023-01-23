#pragma once

#include "base.hpp"

#include <vector>

class Screen2DEffectsType : protected BaseHack
{
public:
	constexpr Screen2DEffectsType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll();
protected:
	virtual void run() = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void load() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
	virtual void roundRestart() override {};
private:
	inline static std::vector<Screen2DEffectsType*> m_hacksRun;
};