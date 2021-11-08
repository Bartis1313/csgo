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
	const auto end = myEye + math::angleVec(cmd->m_viewangles) * weapon->getWpnInfo()->m_range;
	
	trace_t trace;
	ray_t ray;
	traceFilter filter;

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

	if (entity->clientClass()->m_classID != CCSPlayer)
		return;

	if (entity == game::localPlayer || entity->isDormant() || !entity->isAlive() || entity->m_bGunGameImmunity())
		return;

	cmd->m_buttons |= IN_ATTACK;

}