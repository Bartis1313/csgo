#pragma once

#include "base.hpp"

#include <vector>

class CUserCmd;

class CreateMoveInPredictionType : protected BaseHack
{
public:
	constexpr CreateMoveInPredictionType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(CUserCmd* cmd);
protected:
	virtual void run(CUserCmd* cmd) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<CreateMoveInPredictionType*> m_hacksRun;
};

class CreateMovePrePredictionType : protected BaseHack
{
public:
	constexpr CreateMovePrePredictionType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(CUserCmd* cmd);
protected:
	virtual void run(CUserCmd* cmd) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<CreateMovePrePredictionType*> m_hacksRun;
};

class CreateMovePostPredictionType : protected BaseHack
{
public:
	constexpr CreateMovePostPredictionType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(CUserCmd* cmd);
protected:
	virtual void run(CUserCmd* cmd) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<CreateMovePostPredictionType*> m_hacksRun;
};