#pragma once

#include <classes/createMove.hpp>
#include <SDK/math/Vector.hpp>

class CUserCmd;
class IConVar;

class Movement : protected CreateMovePrePredictionType
{
public:
	constexpr Movement() :
		CreateMovePrePredictionType{}
	{}

protected:
	virtual void run(CUserCmd* cmd) override;
	virtual void init() override;
private:
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/333797-humanised-bhop.html
	void bunnyhop(CUserCmd* cmd);
	void strafe(CUserCmd* cmd);

	IConVar* m_sideSpeed;
};

GLOBAL_FEATURE(Movement);

class MovementFix
{
public:
	static void run(CUserCmd* cmd, const Vec3& oldAngle);
};