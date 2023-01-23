#pragma once

#include "base.hpp"

#include <vector>

class FrameStageType : protected BaseHack
{
public:
	constexpr FrameStageType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(int frame);
protected:
	virtual void run(int frame) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void load() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
	virtual void roundRestart() override {};
private:
	inline static std::vector<FrameStageType*> m_hacksRun;
};