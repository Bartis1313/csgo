#pragma once

#include "../base.hpp"

#include <vector>

class FrameStageType : public BaseHack
{
public:
	virtual void init() {};
	virtual void run(int frame) {};
	static void runAll(int frame);
protected:
	inline static std::vector<FrameStageType*> m_hacksRun;
};