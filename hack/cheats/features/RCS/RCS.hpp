#pragma once

#include <cheats/classes/overrideM.hpp>

class IConVar;

class RCS : protected OverrideMouseType
{
public:
	constexpr RCS() :
		OverrideMouseType{}
	{}

protected:
	virtual void init() override;
	virtual void run(float* x, float* y) override;
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	void prepare(float* x, float* y);
	IConVar* m_scale;
	IConVar* m_yaw;
	IConVar* m_pitch;
};

GLOBAL_FEATURE(RCS);