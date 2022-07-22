#pragma once

#include "../../../SDK/IPrediction.hpp"

#include <functional>

class CUserCmd;

class Prediction final
{
public:
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

inline Prediction prediction;

