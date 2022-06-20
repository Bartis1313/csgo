#include "movement.hpp"

#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/math/Vector.hpp"

#include "../../../config/vars.hpp"
#include "../../game.hpp"
#include "../../../utilities/math/math.hpp"
#include "../../../utilities/utilities.hpp"

enum movetypes
{
	NOCLIP = 8,
	LADDER = 9
};

void Movement::bunnyhop(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bBunnyHop))
		return;

	static bool skip = false;
	static bool jumped = false;

	if (!game::localPlayer)
		return;

	if (auto renderMode = game::localPlayer->m_nRenderMode(); renderMode == NOCLIP || renderMode == LADDER)
		return;

	if (Random::getRandom<int>(0, 100) > config.get<int>(vars.iBunnyHopChance))
		return;

	bool jump = cmd->m_buttons & IN_JUMP;

	if (!jumped && skip)
	{
		skip = false;
		cmd->m_buttons |= IN_JUMP;
	}
	else if (jump)
	{
		if (!game::localPlayer->isInAir())
		{
			jumped = true;
			skip = true;
		}
		else
		{
			cmd->m_buttons &= ~IN_JUMP;
			jumped = true;
		}
	}
	else
	{
		jumped = false;
		skip = false;
	}
}

void Movement::strafe(CUserCmd* cmd)
{
	int mode = config.get<int>(vars.iAutoStrafe);
	if (mode == E2T(MovementStraferMode::OFF))
		return;

	if (!game::localPlayer)
		return;

	if (auto renderMode = game::localPlayer->m_nRenderMode(); renderMode == NOCLIP || renderMode == LADDER)
		return;

	[[maybe_unused]] bool jump = cmd->m_buttons & IN_JUMP;
	bool inAir = game::localPlayer->isInAir();

	if (!game::localPlayer->isMoving())
		return;

	const static auto cl_sidespeed = interfaces::cvar->findVar("cl_sidespeed")->getFloat();
	const float speed = game::localPlayer->m_vecVelocity().length2D();

	switch (mode)
	{
	case E2T(MovementStraferMode::SIDESPEED):
	{
		if (inAir)
			cmd->m_sidemove = cmd->m_mousedx > 1 ? cl_sidespeed : -cl_sidespeed;

		break;
	}
	case E2T(MovementStraferMode::ROTATE):
	{
		const auto rotateStrafe = [&](float rotation, const float forward)
		{
			rotation *= DEG2RAD(1.0f);

			cmd->m_forwardmove = std::cos(rotation) * forward;
			cmd->m_sidemove = std::sin(rotation) * forward;
		};

		if (inAir)
		{
			// https://www.quakeworld.nu/wiki/QW_physics_air
			float idealAngle = std::clamp(RAD2DEG(std::asin(30.0f / speed) * 0.5f), 0.0f, 45.0f);
			float side = cmd->m_commandNumber % 2 ? 1.0f : -1.0f; // or use static -/+
			rotateStrafe((idealAngle - 90.0f) * side, cl_sidespeed);
		}

		break;
	}
	case E2T(MovementStraferMode::DIRECTIONAL):
	{
		if (inAir)
		{
			auto calcDelta = [=]() // https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Proper_auto-strafer
			{
				static float maxSpeed = game::localPlayer->m_flMaxspeed(); // this does not change, or it does? correct me
				//printf("maxspee %f\n", maxSpeed);
				const static auto sv_airaccelerate = interfaces::cvar->findVar(XOR("sv_airaccelerate"));
				float term = 30.0f / sv_airaccelerate->getFloat() / maxSpeed * 100.0f / speed;

				if (term < 1.0f && term > -1.0f)
					return std::acos(term);

				return 0.0f;
			};

			float deltaAir = calcDelta();

			if (deltaAir != 0.0f)
			{
				float yaw = DEG2RAD(cmd->m_viewangles.y);
				auto velocityVec = game::localPlayer->m_vecVelocity();
				float velocityDirection = std::atan2(velocityVec.y, velocityVec.x) - yaw;
				float bestAngleMove = std::atan2(-cmd->m_sidemove, cmd->m_forwardmove);

				auto deltaAngle = [](float first, float second) // used to point out angle to finally calculate, detection of dir
				{
					float delta = first - second;
					float res = std::isfinite(delta) ? std::remainder(delta, math::PI * 2.0f) : 0.0f;

					if (first > second)
					{
						if (res >= math::PI)
							res -= math::PI * 2.0f;
					}
					else
					{
						if (res <= -math::PI)
							res += math::PI * 2.0f;
					}

					return res;
				};

				float delta = deltaAngle(velocityDirection, bestAngleMove);
				float finalMove = delta < 0.0f ? velocityDirection + deltaAir : velocityDirection - deltaAir;

				cmd->m_forwardmove = std::cos(finalMove) * cl_sidespeed;
				cmd->m_sidemove = -std::sin(finalMove) * cl_sidespeed;
			}
		}

		break;
	}
	}
}

void Movement::fix(CUserCmd* cmd, const Vector& oldView)
{
	Vector angle = { 0.0f, oldView.y, 0.0f };
	Vector forward, right, up;
	math::angleVectors(angle, forward, right, up);
	forward.normalize(); right.normalize(); // because those are not yet normalized

	Vector angleNow = { 0.0f, cmd->m_viewangles.y, 0.0f };
	Vector forwardNow, rightNow, upNow;
	math::angleVectors(angleNow, forwardNow, rightNow, upNow);

	Vector forwardOld = forward * cmd->m_forwardmove;
	Vector sideOld = right * cmd->m_sidemove;

	float newForwardMove = forwardOld.dot(forwardNow) + sideOld.dot(forwardNow);
	float newSideMove = forwardOld.dot(rightNow) + sideOld.dot(rightNow);

	cmd->m_forwardmove = newForwardMove;
	cmd->m_sidemove = newSideMove;
}