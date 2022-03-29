#pragma once

#include "../../../SDK/IPrediction.hpp"

#include <functional>

class CUserCmd;

class Prediction final
{
public:
	void addToPrediction(CUserCmd* cmd, const std::function<void()>& fun);
private:
	void start(CUserCmd* cmd);
	void end();
	float curTime;
	float frameTime;
	uintptr_t* predicionRandomSeed = {};
	CMoveData data = {};
};

inline Prediction prediction;

