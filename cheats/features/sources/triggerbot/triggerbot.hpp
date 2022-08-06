#pragma once

#include "../../classes/createMove.hpp"

class Triggerbot : public CreateMoveInPredictionType
{
public:
	Triggerbot()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(CUserCmd* cmd);
private:
	float m_delay;
};

[[maybe_unused]] inline auto g_Triggerbot = Triggerbot{};
