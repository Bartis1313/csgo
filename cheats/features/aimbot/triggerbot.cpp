#include "triggerbot.hpp"
#include "../../game.hpp"

// TODO: triggerbot simple delay
void triggerbot::run(CUserCmd* cmd)
{
	if (!vars::bTriggerbot)
		return;

	if (!interfaces::engine->isInGame())
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (weapon->isNonAimable() || weapon->isEmpty())
		return;

	if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
		return;

	const auto myEye = game::localPlayer->getEyePos();
	const auto range = weapon->getWpnInfo()->m_range;
	const auto end = myEye + math::angleVec(cmd->m_viewangles) * range;

	Trace_t trace;
	Ray_t ray;
	TraceFilter filter;

	filter.m_skip = game::localPlayer;

	ray.initialize(myEye, end);
	interfaces::trace->traceRay(ray, MASK_PLAYER, &filter, &trace);

	if (trace.m_hitgroup == 0)
		return;

	auto entity = trace.m_entity;
	if (!entity)
		return;

	if (game::localPlayer->m_iTeamNum() == entity->m_iTeamNum())
		return;

	if (!entity->isPlayer())
		return;

	if (entity->isDormant())
		return;

	if (!entity->isAlive())
		return;

	if (entity->m_bGunGameImmunity())
		return;

	cmd->m_buttons |= IN_ATTACK;
}