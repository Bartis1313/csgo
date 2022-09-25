#pragma once

#include <classes/createMove.hpp>

class Triggerbot : public CreateMoveInPredictionType
{
public:
	constexpr Triggerbot() :
		CreateMoveInPredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);
};

[[maybe_unused]] inline auto g_Triggerbot = Triggerbot{};
