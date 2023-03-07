#pragma once

#include <cheats/classes/renderableToPresent.hpp>
#include <cheats/classes/createMove.hpp>

#include <vector>
#include <array>

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
	virtual void init() override;
private:
	void drawFps();
	void drawVelocity();

	// prevent issues with dynamic max set
	inline static constexpr size_t MAX_SIZE_PLOTS = 1000;

	// using vector, because it's easier to implement in code (emplace_back)
	// even tho, it has known capacity so no big deal
	std::vector<double> m_VelocityRecords;
	std::vector<double> m_FpsRecords;

	// both plots will use it
	std::array<double, MAX_SIZE_PLOTS> m_sharedXS;


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
