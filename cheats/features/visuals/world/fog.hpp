#pragma once

#include <classes/frameStage.hpp>

class FogController_t;

class FogController : protected FrameStageType
{
public:
	constexpr FogController() :
		FrameStageType{}
	{}

protected:
	virtual void run(int frame) override;
};

GLOBAL_FEATURE(FogController);
