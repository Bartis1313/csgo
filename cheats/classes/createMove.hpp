#pragma once

#include "base.hpp"

#include <vector>

class CUserCmd;

class CreateMoveInPredictionType : public BaseHack
{
public:
	constexpr CreateMoveInPredictionType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(CUserCmd* cmd) {};
	static void runAll(CUserCmd* cmd);
protected:
	inline static std::vector<CreateMoveInPredictionType*> m_hacksRun;
};

class CreateMovePrePredictionType : public BaseHack
{
public:
	constexpr CreateMovePrePredictionType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(CUserCmd* cmd) {};
	static void runAll(CUserCmd* cmd);
protected:
	inline static std::vector<CreateMovePrePredictionType*> m_hacksRun;
};

class CreateMovePostPredictionType : public BaseHack
{
public:
	constexpr CreateMovePostPredictionType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(CUserCmd* cmd) {};
	static void runAll(CUserCmd* cmd);
protected:
	inline static std::vector<CreateMovePostPredictionType*> m_hacksRun;
};