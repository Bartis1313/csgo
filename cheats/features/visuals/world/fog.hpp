#pragma once

#include <classes/frameStage.hpp>

class FogController_t;

class FogController : public FrameStageType
{
public:
	constexpr FogController() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
};

[[maybe_unused]] inline auto g_FogController = FogController{};
