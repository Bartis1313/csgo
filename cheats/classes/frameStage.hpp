#pragma once

#include "base.hpp"

#include <vector>

class FrameStageType : public BaseHack
{
public:
	constexpr FrameStageType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(int frame) {};
	static void runAll(int frame);
protected:
	inline static std::vector<FrameStageType*> m_hacksRun;
};