#pragma once

#include <classes/renderableToPresent.hpp>
#include <classes/createMove.hpp>

#include <deque>

class VelocityGather;
class CUserCmd;

class Plots : public RenderablePresentType
{
public:
	Plots() :
		RenderablePresentType{}
	{}

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

class VelocityGather : public CreateMoveInPredictionType
{
public:
	constexpr VelocityGather() :
		CreateMoveInPredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);
};

[[maybe_unused]] inline auto g_VelocityGather = VelocityGather{};
