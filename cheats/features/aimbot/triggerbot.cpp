#include "triggerbot.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"

#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/vars.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/IEngineTrace.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../utilities/math/math.hpp"

void Triggerbot::run(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bTriggerbot))
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
	// punch is needed to not fallback with bad constant shooting
	const auto end = myEye + math::angleVec(cmd->m_viewangles + game::localPlayer->getAimPunch()) * range;

	// initialize delays, timer api is not needed since game shares this information
	/*static auto delay = std::chrono::high_resolution_clock::now();
	const auto current = std::chrono::high_resolution_clock::now();*/
	static auto delay = interfaces::globalVars->m_realtime;
	const auto current = interfaces::globalVars->m_realtime;

	// because this time is in seconds, so delay must be /1000 (s->ms), when using chrono, you can cast to ms so it's more flexible
	if ((current - delay) < static_cast<float>(config.get<float>(vars.fTriggerDelay) / 1000.0f))
		return;

	Trace_t trace;
	TraceFilter filter;

	filter.m_skip = game::localPlayer();
	interfaces::trace->traceRay({ myEye, end }, MASK_PLAYER, &filter, &trace);

	// so this way we skip time of trace
	delay = current;

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

	//if (!game::localPlayer->isPossibleToSee(entity, entity->getEyePos()))
	//	return;

	// that is bad way
	/*if (delay >= vars::iTriggerDelay)
	{
		delay = 0;
		cmd->m_buttons |= IN_ATTACK;
	}*/

	cmd->m_buttons |= IN_ATTACK;
	delay = 0.0f;
}