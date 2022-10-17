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
#include <config/vars.hpp>
#include <game/globals.hpp>
#include <game/game.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/rand.hpp>

void Aimbot::init()
{
	m_scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"));
}

Vec3 Aimbot::smoothAim(const Vec3& angle, float cfgSmooth)
{
	if (cfgSmooth == 0.0f)
		return angle;

	Vec3 delta = angle;
	Vec3 ret;
	const float smooth = std::min(0.99f, cfgSmooth); // allow 1.0 value to be max
	float acceleration;

	switch (m_config.smoothMode)
	{
	case E2T(SmoothMode::LINEAR):
	{
		ret = angle * cfgSmooth;
		acceleration = cfgSmooth;
	}
	case E2T(SmoothMode::NON_LINEAR):
		const float length = delta.length();
		const float deltaSmooth = 1.0f - smooth;
		// results in constant adding, value will not be slightly different each time
		acceleration = (deltaSmooth) / length * std::exp(deltaSmooth);

		// values higher than 1.0 are invalid
		// because we want to slow it down
		acceleration = std::min(1.0f, acceleration);

		if (acceleration == 1.0f) // smoothing ended, return original
			return angle;

		ret = delta * acceleration; // apply changes
	}

	if (m_config.curveAim)
	{
		Vec3 curved = angle + Vec3(angle[1] * m_config.curveX, angle[0] * m_config.curveY, 0.0f);
		ret = curved * acceleration;
	}

	return ret;
}

bool AimbotTarget_t::isBlackListed() const
{
	// wip
	return true;
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
	const auto punch = (weapon->isRifle() || weapon->isSmg()) ? game::localPlayer->getAimPunch() : Vec3{};

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
	std::sort(m_targets.begin(), m_targets.end(),
		[this](const AimbotTarget_t& lhs, const AimbotTarget_t& rhs) // std::tie won't work in that case
		{
			if (lhs.isBlackListed() ^ rhs.isBlackListed())
				return lhs.isBlackListed();

			return lhs.m_fov < rhs.m_fov;
		});

	auto [player, guid, fov, index, bestHitbox, bestpos] = m_targets.front();

	if (m_config.aimBacktrack)
	{
		int boneID = 8; // HEAD start
		if (auto modelStudio = interfaces::modelInfo->getStudioModel(player->getModel()); modelStudio != nullptr)
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

	angle = smoothAim(angle, m_config.smooth);

	cmd->m_viewangles += angle;
	interfaces::engine->setViewAngles(cmd->m_viewangles);

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
			delay = interfaces::globalVars->m_curtime + (m_config.aimDelayVal / 1000.0f);
		}
		if (m_delay) // if the delay is hit, check time, so when ent died
		{
			if (delay <= interfaces::globalVars->m_curtime)
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
			{
				PlayerInfo_t info;
				interfaces::engine->getPlayerInfo(idx, &info);

				m_targets.emplace_back(AimbotTarget_t{ ent, info.m_steamID64, fov, idx, pos, hitPos });
			}
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
