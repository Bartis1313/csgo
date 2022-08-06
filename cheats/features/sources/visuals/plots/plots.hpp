#pragma once

#include "../../../classes/renderableToPresent.hpp"
#include "../../../classes/createMove.hpp"

#include <deque>

class VelocityGather;
class CUserCmd;

class Plots : public RenderablePresentType
{
public:
	Plots()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();

private:
	void drawFps();
	void drawVelocity();

	std::deque<float> m_VelocityRecords;
	std::deque<float> m_FpsRecords;

	inline static constexpr float RECORDS_SIZE = 300.0f;
	inline static float m_acceptanceVelocity = 1.0;

	friend VelocityGather;
};

[[maybe_unused]] inline auto g_Plots = Plots{};

class VelocityGather : CreateMoveInPredictionType
{
public:
	VelocityGather()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(CUserCmd* cmd);
};

[[maybe_unused]] inline auto g_VelocityGather = VelocityGather{};
