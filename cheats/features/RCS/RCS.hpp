#pragma once

#include <classes/createMove.hpp>

struct Vector;
class IConVar;

class RCS : public CreateMoveInPredictionType
{
public:
	constexpr RCS() :
		CreateMoveInPredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);
private:
	void prepare(CUserCmd* cmd);
	IConVar* m_scale;
};

[[maybe_unused]] inline auto g_Rcs = RCS{};