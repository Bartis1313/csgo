#pragma once

#include <classes/renderableToPresent.hpp>
#include <classes/createMove.hpp>

#include <deque>

class VelocityGather;
class CUserCmd;

class Plots : protected RenderablePresentType
{
public:
	Plots() :
		RenderablePresentType{}
	{}

protected:
	virtual void draw() override;
private:
	void drawFps();
	void drawVelocity();

	std::deque<float> m_VelocityRecords;
	std::deque<float> m_FpsRecords;

	inline static constexpr float RECORDS_SIZE = 300.0f;
	inline static float m_acceptanceVelocity = 1.0;

	friend VelocityGather;
};

GLOBAL_FEATURE(Plots);

class VelocityGather : protected CreateMoveInPredictionType
{
public:
	constexpr VelocityGather() :
		CreateMoveInPredictionType{}
	{}

protected:
	virtual void run(CUserCmd* cmd) override;
};

GLOBAL_FEATURE(VelocityGather);
