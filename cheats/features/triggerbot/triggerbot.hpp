#pragma once

#include <classes/createMove.hpp>

class Triggerbot : protected CreateMoveInPredictionType
{
public:
	constexpr Triggerbot() :
		CreateMoveInPredictionType{}
	{}

protected:
	virtual void run(CUserCmd* cmd) override;
};

GLOBAL_FEATURE(Triggerbot);
