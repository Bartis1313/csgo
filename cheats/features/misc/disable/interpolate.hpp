#pragma once

#include <classes/frameStage.hpp>

class Interpolate : protected FrameStageType
{
public:
	constexpr Interpolate()
		: FrameStageType{}
	{}

protected:
	virtual void run(int frame) override;
};