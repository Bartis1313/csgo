#include "enemyWarn.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>

namespace enemyWarning
{
	using checkType = std::optional<std::pair<bool, bool>>;
	checkType check(Player_t* ent);

	std::array<checkType, 65> spottedEnts;
}

enemyWarning::checkType enemyWarning::check(Player_t* ent)
{
	if (!game::isAvailable())
		return std::nullopt;

	auto posDelta = ent->getEyePos() - game::localPlayer->getEyePos();
	/*Vector idealAimAngle = math::vectorToAngle(posDelta);

	idealAimAngle -= ent->m_aimPunchAngle() * m_scale->getFloat();

	Vector curEnemyAngle = ent->m_angEyeAngles();
	curEnemyAngle.normalize();*/

	auto forward = math::angleVec(game::localPlayer->m_angEyeAngles());
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
	memory::interfaces::trace->traceRay({ eye, end }, MASK_PLAYER, &filter, &trace);

	bool checkAim = trace.m_hitgroup != 0;

	return std::pair{ check, checkAim };
}

void enemyWarning::beginThink(Player_t* ent)
{
	if (!vars::misc->aimWarn->enabled)
		return;

	spottedEnts.at(ent->getIndex()) = check(ent);
}

void enemyWarning::endThink()
{
	for (auto& spot : spottedEnts)
		spot = std::nullopt;
}

void enemyWarning::draw()
{
	if (!vars::misc->aimWarn->enabled)
		return;

	bool visible{ false };
	bool aiming{ false };
	for (const auto& spot : spottedEnts)
	{
		if (spot.has_value())
		{
			const auto [vis, aim] = spot.value();
			if (vis)
				visible = true;

			if (aim)
				aiming = true;
		}
	}

	if(visible)
		ImRender::text(globals::screenX / 2.0f, 60.0f, ImRender::fonts::tahoma14, "Enemy can see you", true, Colors::Green);
	if(aiming)
		ImRender::text(globals::screenX / 2.0f, 80.0f, ImRender::fonts::tahoma14, "Enemy is aiming you", true, Colors::Red);
}