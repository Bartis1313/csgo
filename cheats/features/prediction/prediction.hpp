#pragma once

#include <classes/onlyInit.hpp>
#include <SDK/IPrediction.hpp>

#include <functional>

class CUserCmd;

class Prediction : protected OnlyInitType
{
public:
	constexpr Prediction() :
		OnlyInitType{}
	{}

	void addToPrediction(CUserCmd* cmd, const std::function<void()>& fun);
	void update();
protected:
	virtual void init() override;
private:
	void start(CUserCmd* cmd);
	void end();
	float m_curTime;
	float m_frameTime;
	uintptr_t* m_predicionRandomSeed = {};
	CMoveData* m_data = {};
};

GLOBAL_FEATURE(Prediction);
