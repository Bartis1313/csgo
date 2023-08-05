#include "aimbot.hpp"

#include "../RCS/RCS.hpp"
#include "helper.hpp"
#include <cheats/features/cache/cache.hpp>
#include <cheats/features/backtrack/backtrack.hpp>

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

#include <cheats/hooks/createMove.hpp>
#include <cheats/hooks/overrideMouse.hpp>
#include <cheats/hooks/wndproc.hpp>
#include <cheats/hooks/createMove.hpp>

namespace
{
	struct HandlerAim : hooks::OverrideMouse
	{
		HandlerAim()
		{
			this->registerRun(Aimbot::runMouse);
		}
	} handlerAim;

	struct HandlerAimKeys : hooks::wndProcSys
	{
		HandlerAimKeys()
		{
			this->registerRun(Aimbot::updateKeys);
		}
	} handlerAimKeys;

	struct HandlerAimCM : hooks::CreateMove
	{
		HandlerAimCM()
		{
			this->registerInit(Aimbot::init);
			this->registerRunPrediction(Aimbot::run);
		}
	} handlerAimCM;
}

namespace Aimbot
{
	struct AimbotTarget_t
	{
		Player_t* player{ };
		float fov{ };
		Hitboxes hitbox{ };
		Vec3 pos{ };
	};

	void resetFields();
	bool isClicked(CUserCmd* cmd);
	void addBacktrack();
	void sortTargets();
	bool addDelay();
	void addTargets(CUserCmd* cmd, Weapon_t* wpn, const Vec3& eye, const Vec3& punch);
	std::vector<Hitboxes> getHitboxes();

	bool inDelay{ false };
	Vec3 toAdd{ };
	bool shouldWork{ false };
	std::mutex mutex;
	std::vector<AimbotTarget_t> targets;
	CfgWeapon cfgWeapon;

	IConVar* weapon_recoil_scale{ };
	IConVar* m_yaw{ };
	IConVar* m_pitch{ };

	bool inited{ false };
}

void Aimbot::init()
{
	weapon_recoil_scale = memory::interfaces::cvar->findVar("weapon_recoil_scale");
	m_yaw = memory::interfaces::cvar->findVar("m_yaw");
	m_pitch = memory::interfaces::cvar->findVar("m_pitch");
}

void Aimbot::updateKeys()
{
	cfgWeapon.key.update();
}

void Aimbot::addBacktrack()
{
	if (cfgWeapon.aimBacktrack)
	{
		if (targets.empty())
			return;

		auto& [player, fov, bestHitbox, bestpos] = targets.front();

		int boneID = 8; // HEAD start
		if (auto modelStudio = memory::interfaces::modelInfo->getStudioModel(player->getModel()); modelStudio != nullptr)
		{
			if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(bestHitbox); hitbox != nullptr)
			{
				boneID = hitbox->m_bone;
			}
		}
		const auto& record = backtrack::records.at(player->getIndex());
		bestpos = record.at(record.size() / 2).matrices[boneID].origin(); // middle, u can play with this as u want to
	}
}

void Aimbot::sortTargets()
{
	// everything sorted by fov. Blacklists go on top + they ofc are sorted
	// we always want front() as current target
	std::ranges::sort(targets,
		[](const AimbotTarget_t& lhs, const AimbotTarget_t& rhs) // std::tie won't work in that case
		{
			if (blacklist::isBlacklisted(lhs.player) ^ blacklist::isBlacklisted(rhs.player))
				return blacklist::isBlacklisted(lhs.player);

			return lhs.fov < rhs.fov;
		});
}

bool Aimbot::addDelay()
{
	if (targets.empty())
		return true;

	static float delay{ 0.0f };
	// will not work for the special case:
	// delay did not hit timer limit but we switched manually to new target -> should reset the timer. I couldn't detect it without false positives :(
	// epic solution - stop shooting and start again
	if (cfgWeapon.aimDelay)
	{
		if (targets.size() && !inDelay && !targets.front().player->isAlive()) // if ent is found and dead, then set field to delay and wait curr time + cfgtime
		{
			inDelay = true;
			delay = memory::interfaces::globalVars->m_curtime + (cfgWeapon.aimDelayVal / 1000.0f);
		}
		if (inDelay) // if the delay is hit, check time, so when ent died
		{
			if (delay <= memory::interfaces::globalVars->m_curtime)
				inDelay = false;
			else // need reset fields here and stop the method
			{
				targets.clear();
				return false;
			}
		}
	}

	return true;
}

void Aimbot::runMouse(float* x, float* y)
{
	std::lock_guard lock{ mutex };

	if (!shouldWork)
		return;

	Vec2 mouse = Vec2{ *x, *y };
	Vec2 mouseScreen = Vec2{ toAdd[1], -toAdd[0] };

	mouseScreen += mouse;

	*x = mouseScreen[0];
	*y = mouseScreen[1];
}

void Aimbot::run(CUserCmd* cmd)
{
	std::lock_guard lock{ mutex };

	if (!inited)
		return;

	if (!game::isAvailable())
		return;

	shouldWork = false;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	const auto maybeCfg = configWeapon::get();
	if (!maybeCfg.has_value())
		return;

	cfgWeapon = maybeCfg.value();

	if (!cfgWeapon.enabled)
		return;

	if (weapon->isNonAimable())
	{
		resetFields(); // switching to knife when clicked
		return;
	}

	if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
		return;

	if (!isClicked(cmd))
	{
		resetFields();
		return;
	}

	const Vec3 myEye = game::localPlayer->getEyePos();
	Vec3 punch{ };
	if (game::localPlayer->m_iShotsFired())
	{
		punch = (weapon->isRifle() || weapon->isSmg()) ? game::localPlayer->getAimPunch() : Vec3{};
		if (cfgWeapon.rcs)
			punch = rcs::lastPunch;
	}

	addTargets(cmd, weapon, myEye, punch);
	sortTargets();

	if (targets.empty())
		return;

	const auto [player, fov, bestHitbox, bestpos] = targets.front();

	const auto currentAngle = Vec3{ cmd->m_viewangles + punch };
	const auto& angle = math::calcAngle(myEye, bestpos);
	float smoothingFactor = std::min(memory::interfaces::globalVars->m_frametime * cfgWeapon.frametimeMulttiply, 1.0f);
	const auto& lerpedAngle = currentAngle.lerp(angle, std::clamp(smoothingFactor, 0.01f, 1.0f)).normalize().clamp();

	toAdd = Vec3{ currentAngle - lerpedAngle };

	toAdd[0] /= m_pitch->getFloat();
	toAdd[1] /= m_yaw->getFloat();

	shouldWork = true;
}

// because for any reason optimizations cause deadlock
#pragma optimize("", off)
void Aimbot::addTargets(CUserCmd* cmd, Weapon_t* wpn, const Vec3& eye, const Vec3& punch)
{
	if (!addDelay())
		return;

	targets.clear();

	if (game::localPlayer->m_flFlashDuration() > 0.0f)
	{
		if (game::localPlayer->m_flFlashBangTime() >= cfgWeapon.flashLimit)
			return;
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

		float bestFov{ cfgWeapon.fov };
		Vec3 bestPos{ };
		Hitboxes bestHitbox{ HITBOX_HEAD };
		bool readyToAdd{ false };

		for (const auto hitbox : getHitboxes())
		{
			const Vec3 hitPos = ent->getHitboxPos(hitbox);
			const Vec3 angles = cmd->m_viewangles + punch;

			const auto fov = cfgWeapon.methodAim == E2T(AimbotMethod::CROSSHAIR)
				? math::calcFov(eye, hitPos, angles)
				: math::calcFovReal(eye, hitPos, angles);

			if (fov > bestFov)
				continue;

			if (!game::localPlayer->isPossibleToSee(ent, hitPos))
				continue;

			if (cfgWeapon.smokeCheck && game::localPlayer->isViewInSmoke(hitPos))
				continue;

			if (fov < bestFov)
			{
				bestFov = fov;
				bestHitbox = hitbox;
				bestPos = hitPos;

				readyToAdd = true;
			}
		}

		if (readyToAdd)
		{
			targets.emplace_back(ent, bestFov, bestHitbox, bestPos);
		}
	}

	addBacktrack();
}
#pragma optimize("", on)

void Aimbot::resetFields()
{
	targets.clear();
	inDelay = false;
}

bool Aimbot::isClicked(CUserCmd* cmd)
{
	if (cfgWeapon.useKey)
	{
		if (cfgWeapon.key.isEnabled())
			return true;
		else
			return false;
	}
	else
		return cmd->m_buttons & IN_ATTACK;

	return false;
}

std::vector<Hitboxes> Aimbot::getHitboxes()
{
	static std::vector<Hitboxes> allHitboxes(HITBOX_MAX);
	static std::once_flag once;
	std::call_once(once, [&]()
	{
		// iota won't work, so doing it manually
		for (int i = HITBOX_HEAD; i < HITBOX_MAX; i++)
			allHitboxes.at(i) = static_cast<Hitboxes>(i);
	});
	static std::vector<Hitboxes> headHitbox{ HITBOX_HEAD };
	static std::vector<Hitboxes> chestHitbox{ HITBOX_LOWER_CHEST, HITBOX_UPPER_CHEST };


	switch (cfgWeapon.aimSelection)
	{
	case E2T(AimbotHitboxes::NEAREST):
		return allHitboxes;
	case E2T(AimbotHitboxes::HEAD):
		return headHitbox;
	case E2T(AimbotHitboxes::CHEST):
		return chestHitbox;
	}

	return {};
}