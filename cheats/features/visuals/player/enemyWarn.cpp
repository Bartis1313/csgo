#include "enemyWarn.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/tools.hpp>


void EnemyWarning::init()
{
	m_scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"));
}

std::pair<bool, bool> EnemyWarning::check(Player_t* ent)
{
	if (!config.get<bool>(vars.bAimingWarn))
		return { false, false };

	if (!game::isAvailable())
		return { false, false };

	Vector posDelta = ent->getEyePos() - game::localPlayer->getEyePos();
	/*Vector idealAimAngle = math::vectorToAngle(posDelta);

	idealAimAngle -= ent->m_aimPunchAngle() * m_scale->getFloat();

	Vector curEnemyAngle = ent->m_angEyeAngles();
	curEnemyAngle.normalize();*/

	Vector forward = math::angleVec(game::localPlayer->m_angEyeAngles());
	// yeah that's hardcoded and not 100% accurate, I thought of something like in radar as "fov view", but we can't be sure if enemy uses fov changer
	bool isBehind = posDelta.dot(forward) < 15.0f;

	bool check = ent->isPossibleToSee(game::localPlayer(), game::localPlayer->getEyePos()) && !isBehind;

	// dynamic fov
	// float fov = math::calcFovReal(ent->getEyePos(), game::localPlayer->getBonePos(3), curEnemyAngle); // 3 is middle body

	Trace_t trace;
	TraceFilter filter{ ent };
	const auto eye = ent->getEyePos();
	constexpr float range = 8192.0f; // because we need max range possible
	const auto end = eye + (math::angleVec(ent->m_angEyeAngles()) * range);
	interfaces::trace->traceRay({ eye, end }, MASK_PLAYER, &filter, &trace);

	bool checkAim = trace.m_hitgroup != 0;

	return { check, checkAim };
}

void EnemyWarning::draw(const std::pair<bool, bool>& checks)
{
	if(checks.first) // trace
		imRender.text(globals::screenX / 2.0f, 60.0f, ImFonts::tahoma14, XOR("Enemy can see you"), true, Colors::Green);
	if(checks.second) // dynamic fov
		imRender.text(globals::screenX / 2.0f, 80.0f, ImFonts::tahoma14, XOR("Enemy is aiming you"), true, Colors::Red);
}