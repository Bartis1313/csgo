#pragma once

#include <classes/createMove.hpp>
#include <SDK/math/Vector.hpp>

class CUserCmd;
class IConVar;

class Movement : public CreateMovePrePredictionType
{
public:
	constexpr Movement() :
		CreateMovePrePredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);
private:
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/333797-humanised-bhop.html
	void bunnyhop(CUserCmd* cmd);
	void strafe(CUserCmd* cmd);

	IConVar* m_sideSpeed;
};

[[maybe_unused]] inline auto g_Movement = Movement{};

class MovementFix
{
public:
	virtual void run(CUserCmd* cmd, const Vector& oldAngle);
};

[[maybe_unused]] inline auto g_MovementFix = MovementFix{};