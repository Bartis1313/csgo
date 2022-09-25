#pragma once

#include <classes/onlyInit.hpp>
#include <SDK/IPrediction.hpp>

#include <functional>

class CUserCmd;

class Prediction : public OnlyInitType
{
public:
	constexpr Prediction() :
		OnlyInitType{}
	{}

	virtual void init();
	void addToPrediction(CUserCmd* cmd, const std::function<void()>& fun);
	void update();
private:
	void start(CUserCmd* cmd);
	void end();
	float m_curTime;
	float m_frameTime;
	uintptr_t* m_predicionRandomSeed = {};
	CMoveData* m_data = {};
};

[[maybe_unused]] inline auto g_Prediction = Prediction{};
