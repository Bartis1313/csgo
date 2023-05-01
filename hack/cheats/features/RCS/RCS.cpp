#include "RCS.hpp"

#include "../aimbot/cmdCache.hpp"
#include "../aimbot/aimbot.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/CGameMovement.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

#include <cheats/game/game.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

#include <cheats/hooks/overrideMouse.hpp>

namespace
{
	struct RcsHandler : hooks::OverrideMouse
	{
		RcsHandler()
		{
			this->registerInit(rcs::init);
			this->registerRun(rcs::run);
		}
	} rcsHandler;
}

namespace rcs
{
	IConVar* scale{ };
	IConVar* yaw{ };
	IConVar* pitch{ };
	CfgWeapon cfg{ };

	void prepare(float* x, float* y);
}

void rcs::init()
{
	scale = memory::interfaces::cvar->findVar("weapon_recoil_scale");
	yaw = memory::interfaces::cvar->findVar("m_yaw");
	pitch = memory::interfaces::cvar->findVar("m_pitch");
}

void rcs::run(float* x, float* y)
{
	auto maybeConfig = CUserCmdCache::getWeaponConfig();
	if (!maybeConfig.has_value())
		return;

	cfg = maybeConfig.value();

	if (!cfg.rcs)
		return;

	if (!game::isAvailable())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();

	if (!weapon)
		return;

	if (weapon->isNonAimable())
		return;

	if (weapon->isSniper())
		return;

	prepare(x, y);
}

void rcs::prepare(float* x, float* y)
{
	const auto cmd = CUserCmdCache::getCmd();
	if (!cmd)
		return;

	Vec3 punch = game::localPlayer->getAimPunch();
	punch *= scale->getFloat();
	punch[2] = 0.0f;

	static auto oldPunch = punch;
	if (game::localPlayer()->m_iShotsFired() > 1 && cmd->m_buttons & IN_ATTACK)
	{
		const Vec3 deltaPunch = punch - oldPunch;
		constexpr int steps = 10;
		const float step = punch.length() / steps;
		// do smth better with length? this is cheap solution to dont overwrite the aimangle so obviously at like single-patch
		// yet still control the recoil perfectly
		// maybe interpolate with pix?
		for (float pix = 0.0f; pix <= std::min(1.0f, punch.length()); pix += step)
		{
			const Vec3 punchPos = game::getViewAngles() - (deltaPunch * (1.0f - pix));
			auto& toAdd = Vec3{ game::getViewAngles() - punchPos }.normalize().clamp();

			toAdd[0] /= pitch->getFloat();
			toAdd[1] /= yaw->getFloat();

			Vec2 mouse = Vec2{ *x, *y };
			Vec2 mouseScreen = Vec2{ toAdd[1], -toAdd[0] };

			mouseScreen[0] = (mouseScreen[0] + mouse[0]) / 2.0f;
			mouseScreen[1] = (mouseScreen[1] + mouse[1]) / 2.0f;

			*x = mouseScreen[0];
			*y = mouseScreen[1];
		}

		oldPunch = punch;
	}
	else
	{
		oldPunch = punch;
	}
}
