#include "movement.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/rand.hpp>

enum movetypes
{
	NOCLIP = 8,
	LADDER = 9
};

void Movement::init()
{
	m_sideSpeed = interfaces::cvar->findVar(XOR("cl_sidespeed"));
}

void Movement::run(CUserCmd* cmd)
{
	bunnyhop(cmd);
	strafe(cmd);
}

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

	const bool jump = cmd->m_buttons & IN_JUMP;

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
	const int mode = config.get<int>(vars.iAutoStrafe);
	if (mode == E2T(MovementStraferMode::OFF))
		return;

	if (!game::localPlayer)
		return;

	if (auto renderMode = game::localPlayer->m_nRenderMode(); renderMode == NOCLIP || renderMode == LADDER)
		return;

	[[maybe_unused]] const bool jump = cmd->m_buttons & IN_JUMP;
	const bool inAir = game::localPlayer->isInAir();

	if (!game::localPlayer->isMoving())
		return;

	const float speed = game::localPlayer->m_vecVelocity().length2D();

	switch (mode)
	{
	case E2T(MovementStraferMode::SIDESPEED):
	{
		if (inAir)
			cmd->m_sidemove = cmd->m_mousedx > 1 ? m_sideSpeed->getFloat() : -m_sideSpeed->getFloat();

		break;
	}
	case E2T(MovementStraferMode::ROTATE):
	{
		auto rotateStrafe = [=](float rotation, const float forward)
		{
			rotation *= math::DEG2RAD(1.0f);

			cmd->m_forwardmove = std::cos(rotation) * forward;
			cmd->m_sidemove = std::sin(rotation) * forward;
		};

		if (inAir)
		{
			// https://www.quakeworld.nu/wiki/QW_physics_air
			const float idealAngle = std::clamp(math::RAD2DEG(std::asin(30.0f / speed) * 0.5f), 0.0f, 45.0f);
			const float side = cmd->m_commandNumber % 2 ? 1.0f : -1.0f; // or use static -/+
			rotateStrafe((idealAngle - 90.0f) * side, m_sideSpeed->getFloat());
		}

		break;
	}
	case E2T(MovementStraferMode::DIRECTIONAL):
	{
		if (inAir)
		{
			auto calcDelta = [=]() // https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Proper_auto-strafer
			{
				const static float maxSpeed = game::localPlayer->m_flMaxspeed(); // this does not change, or it does? correct me
				//printf("maxspee %f\n", maxSpeed);
				const static auto sv_airaccelerate = interfaces::cvar->findVar(XOR("sv_airaccelerate"));
				const float term = 30.0f / sv_airaccelerate->getFloat() / maxSpeed * 100.0f / speed;

				if (term < 1.0f && term > -1.0f)
					return std::acos(term);

				return 0.0f;
			};

			const float deltaAir = calcDelta();

			if (deltaAir != 0.0f)
			{
				const float yaw = math::DEG2RAD(cmd->m_viewangles.y);
				const auto velocityVec = game::localPlayer->m_vecVelocity();
				const float velocityDirection = std::atan2(velocityVec.y, velocityVec.x) - yaw;
				const float bestAngleMove = std::atan2(-cmd->m_sidemove, cmd->m_forwardmove);

				auto deltaAngle = [](float first, float second) // used to point out angle to finally calculate, detection of dir
				{
					const float delta = first - second;
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

				const float delta = deltaAngle(velocityDirection, bestAngleMove);
				const float finalMove = delta < 0.0f ? velocityDirection + deltaAir : velocityDirection - deltaAir;

				cmd->m_forwardmove = std::cos(finalMove) * m_sideSpeed->getFloat();
				cmd->m_sidemove = -std::sin(finalMove) * m_sideSpeed->getFloat();
			}
		}

		break;
	}
	}
}

void MovementFix::run(CUserCmd* cmd, const Vector& oldAngle)
{
	const Vector angle = { 0.0f, oldAngle.y, 0.0f };
	auto [forward, right, up] = math::angleVectors(angle);
	forward.normalize(); right.normalize(); // because those are not yet normalized

	const Vector angleNow = { 0.0f, cmd->m_viewangles.y, 0.0f };
	auto [forwardNow, rightNow, upNow] = math::angleVectors(angleNow);

	const Vector forwardOld = forward * cmd->m_forwardmove;
	const Vector sideOld = right * cmd->m_sidemove;

	const float newForwardMove = forwardOld.dot(forwardNow) + sideOld.dot(forwardNow);
	const float newSideMove = forwardOld.dot(rightNow) + sideOld.dot(rightNow);

	cmd->m_forwardmove = newForwardMove;
	cmd->m_sidemove = newSideMove;
}