#include "aimbot.hpp"

#include "../cache/cache.hpp"
#include "../backtrack/backtrack.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/vars.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>
#include <cheats/game/globals.hpp>
#include <cheats/game/game.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/rand.hpp>
#include <cheats/features/blacklist/blacklist.hpp>

void Aimbot::init()
{
	m_scale = memory::interfaces::cvar->findVar("weapon_recoil_scale");
}

Vec3 Aimbot::smoothAim(CUserCmd* cmd, const Vec3& angle, Player_t* target, float cfgSmooth)
{
	if (cfgSmooth == 0.0f)
		return angle;

	// velocity, distance, mouse deltas are scales
	// skill is taken as global multiply for those values
	// this isnt perfect. perfect is to record dataset of yourself playing, 30mins of aiming is enough
	// use this dataset, if possible get dataset of friend that plays very well
	// compare differences and apply values that match your assist in best way
	const float smooth = std::clamp(cfgSmooth, 0.0f, 1.0f);
	const float skill = m_config.skill;
	const float targetVel = target->m_vecVelocity().length();
	const float maxVel = target->m_flMaxspeed();
	const float velScaled = std::clamp(targetVel / maxVel, 0.0f, 1.0f);
	const float distance = target->m_vecOrigin().distTo(game::localPlayer->m_vecOrigin());
	const float distanceScaled = std::clamp(distance / 1000.0f, 0.0f, 1.0f);
	const float mousedx = static_cast<float>(cmd->m_mousedx) / (globals::screenX / 4);
	const float mousedy = static_cast<float>(cmd->m_mousedy) / (globals::screenY / 4);
	smoothFactor = std::min(std::lerp(smooth, 1.0f, 
		(velScaled * 0.2f + distanceScaled * 0.1f + mousedx * 0.5f + mousedy * 0.5f) * skill), 1.0f);

	Vec3 delta = angle;
	Vec3 ret;
	switch (m_config.smoothMode)
	{
	case E2T(SmoothMode::LINEAR):
		ret = angle * (1.0f - smoothFactor);
		break;
	case E2T(SmoothMode::AIM_LENGTH):
		ret = delta - (delta * smoothFactor);
		break;
	case E2T(SmoothMode::AIM_CUBIC):
	{
		const float t = 1.0f - smoothFactor;
		const float cubic = t * t * (3.0f - 2.0f * t);
		ret = delta * cubic;
		break;
	}
	}

	const bool movingMouse = cmd->m_mousedx > 0 || cmd->m_mousedy > 0;
	if (m_config.curveAim && !movingMouse)
	{
		Vec3 curved = ret + Vec3(ret[1] * m_config.curveX, ret[0] * m_config.curveY, 0.0f);
		ret = curved * smoothFactor;
	}
	return ret;
}

float Aimbot::getRandomizedSmooth(float currentSmooth)
{
	const float sineWave = std::sin(static_cast<float>(memory::interfaces::globalVars->m_tickCount));
	const float factor = sineWave * m_config.randomizationRatio;
	const float smoothness = currentSmooth + factor;
	return currentSmooth *= smoothness;
}

void Aimbot::run(CUserCmd* cmd)
{
	m_view = cmd->m_viewangles;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;


	auto weaponCfg = CfgWeapon::getWeaponByIndex(weapon->m_iItemDefinitionIndex());
	if (weaponCfg == WeaponList::WEAPON_UNKNOWN)
		return;

	auto indexCfg = E2T(weaponCfg);
	m_config = vars::aim->weapons.at(indexCfg);

	if (!m_config.enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	if (weapon->isNonAimable())
	{
		resetFields(); // switching to knife when clicked
		return;
	}

	if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
		return;

	const auto myEye = game::localPlayer->getEyePos();
	const auto punch = (weapon->isRifle() || weapon->isSmg()) ? game::getFixedPunch() : Vec3{};

	if (!isClicked(cmd))
	{
		resetFields();
		return;
	}

	if (!getBestTarget(cmd, weapon, myEye, punch))
		return;

	if (m_targets.front().m_pos.isZero())
		return;

	// everything sorted by fov. Blacklists go on top + they ofc are sorted
	// we always want front() as current target
	std::ranges::sort(m_targets,
		[this](const AimbotTarget_t& lhs, const AimbotTarget_t& rhs) // std::tie won't work in that case
		{
			if (g_Blacklist->isBlacklisted(lhs.m_player) ^ g_Blacklist->isBlacklisted(rhs.m_player))
			return g_Blacklist->isBlacklisted(lhs.m_player);

	return lhs.m_fov < rhs.m_fov;
		});

	auto [player, fov, index, bestHitbox, bestpos] = m_targets.front();

	if (m_config.aimBacktrack)
	{
		int boneID = 8; // HEAD start
		if (auto modelStudio = memory::interfaces::modelInfo->getStudioModel(player->getModel()); modelStudio != nullptr)
		{
			if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(bestHitbox); hitbox != nullptr)
			{
				boneID = hitbox->m_bone;
			}
		}
		auto record = g_Backtrack->getAllRecords().at(player->getIndex());
		bestpos = record.at(record.size() / 2).m_matrix[boneID].origin(); // middle, u can play with this as u want to
	}

	auto angle = math::calcAngleRelative(myEye, bestpos, Vec3{ m_view + punch });
	angle.clamp();

	angle = smoothAim(cmd, angle, player, m_config.smooth);

	cmd->m_viewangles += angle;
	memory::interfaces::engine->setViewAngles(cmd->m_viewangles);

}

bool Aimbot::getBestTarget(CUserCmd* cmd, Weapon_t* wpn, const Vec3& eye, const Vec3& punch)
{
	/*bool isSame = m_temp == m_bestEnt;
	console.print("{}\n", isSame);*/

	static float delay = 0.0f;
	// will not work for the special case:
	// delay did not hit timer limit but we switched manually to new target -> should reset the timer. I couldn't detect it without false positives :(
	// epic solution - stop shooting and start again
	if (m_config.aimDelay)
	{
		/*if (m_bestEnt && m_bestEnt->isAlive())
		{
			m_temp = m_bestEnt;
		}*/
		if (m_targets.size() && !m_delay && !m_targets.front().m_player->isAlive()) // if ent is found and dead, then set field to delay and wait curr time + cfgtime
		{
			m_delay = true;
			delay = memory::interfaces::globalVars->m_curtime + (m_config.aimDelayVal / 1000.0f);
		}
		if (m_delay) // if the delay is hit, check time, so when ent died
		{
			if (delay <= memory::interfaces::globalVars->m_curtime)
				m_delay = false;
			else // need reset fields here and stop the method
			{
				m_targets.clear();
				return false;
			}
		}
	}

	m_targets.clear();

	auto hitboxes = getHitboxes();

	if (game::localPlayer->m_flFlashDuration() > 0.0f)
	{
		if (game::localPlayer->m_flFlashBangTime() >= m_config.flashLimit)
			return false;
	}

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		if (!ent)
			continue;

		if (ent == game::localPlayer)
			continue;

		if (!ent->isAlive() || !game::localPlayer->isAlive())
			continue;

		if (!ent->isOtherTeam(game::localPlayer()))
			continue;

		if (ent->isDormant())
			continue;

		if (ent->m_bGunGameImmunity())
			continue;

		for (const auto pos : hitboxes)
		{
			Vec3 hitPos = ent->getHitboxPos(pos);

			auto angles = cmd->m_viewangles + punch;
			angles.clamp();

			if (!game::localPlayer->isPossibleToSee(ent, hitPos))
				continue;

			if (m_config.smokeCheck && game::localPlayer->isViewInSmoke(hitPos))
				continue;

			auto fov = m_config.methodAim == E2T(AimbotMethod::CROSSHAIR)
				? math::calcFov(eye, hitPos, angles)
				: math::calcFovReal(eye, hitPos, angles);

			if (fov <= m_config.fov)
				m_targets.emplace_back(AimbotTarget_t{ ent, fov, idx, pos, hitPos });
		}
	}

	return m_targets.size();
}

Player_t* Aimbot::getTargetted() const
{
	return m_bestEnt;
}

Vec3 Aimbot::getCachedView() const
{
	return m_view;
}

Vec3 Aimbot::getBestHibox() const
{
	return m_bestHitpos;
}

CfgWeapon Aimbot::getCachedConfig() const
{
	return m_config;
}

void Aimbot::resetFields()
{
	m_targets.clear();
	m_delay = false;
}

bool Aimbot::isClicked(CUserCmd* cmd)
{
	if (vars::aim->useKey)
	{
		if (vars::keys->aimbot.isEnabled())
			return true;
		else
			return false;
	}
	else
		return cmd->m_buttons & IN_ATTACK;
}

std::vector<size_t> Aimbot::getHitboxes()
{
	std::vector<size_t> hitboxes;

	switch (m_config.aimSelection)
	{
	case E2T(AimbotHitboxes::NEAREST):
	{
		for (size_t i = HITBOX_HEAD; i < HITBOX_MAX; i++)
			hitboxes.push_back(i);

		break;
	}
	case E2T(AimbotHitboxes::HEAD):
		hitboxes = { HITBOX_HEAD };
		break;
	case E2T(AimbotHitboxes::CHEST):
		hitboxes = { HITBOX_LOWER_CHEST };
		break;
	default:
		break;
	}

	return hitboxes;
}
