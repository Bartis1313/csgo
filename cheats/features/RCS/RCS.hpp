#pragma once

#include <classes/createMove.hpp>

class IConVar;

class RCS : protected CreateMoveInPredictionType
{
public:
	constexpr RCS() :
		CreateMoveInPredictionType{}
	{}

protected:
	virtual void init() override;
	virtual void run(CUserCmd* cmd) override;
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	void prepare(CUserCmd* cmd);
	IConVar* m_scale;
};

GLOBAL_FEATURE(RCS);