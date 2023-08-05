#include "RCS.hpp"

#include "../aimbot/aimbot.hpp"
#include "../aimbot/helper.hpp"

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
#include <cheats/hooks/createMove.hpp>

namespace
{
	struct RcsHandler : hooks::OverrideMouse
	{
		RcsHandler()
		{
			this->registerRun(rcs::runMouse);
		}
	} rcsHandler;

	struct RcsHandlerCM : hooks::CreateMove
	{
		RcsHandlerCM()
		{
			this->registerInit(rcs::init);
			this->registerRunPrediction(rcs::run);
		}
	} rcsHandlerCM;
}

namespace rcs
{
	IConVar* weapon_recoil_scale{ };
	IConVar* m_yaw{ };
	IConVar* m_pitch{ };
	CfgWeapon cfg{ };

	bool shouldWork{ };
	float scale{ };
	std::mutex mutex{ };
	CUserCmd* m_cmd{ };

	bool inited{ false };
}

void rcs::init()
{
	weapon_recoil_scale = memory::interfaces::cvar->findVar("weapon_recoil_scale");
	m_yaw = memory::interfaces::cvar->findVar("m_yaw");
	m_pitch = memory::interfaces::cvar->findVar("m_pitch");

	inited = true;
}

void rcs::runMouse(float* x, float* y)
{
	std::lock_guard lock{ mutex };

	if (!inited)
		return;

	if (!shouldWork)
		return;

	if (m_cmd->m_buttons & IN_ATTACK)
	{
		if (game::localPlayer()->m_iShotsFired() > 2)
		{
			const Vec3 view = m_cmd->m_viewangles;
			const Vec3 punch = game::localPlayer->getAimPunch();

			const Vec3 delta = punch - lastPunch;
			const Vec3 addon{ delta[0] * scale * cfg.rcsX, delta[1] * scale * cfg.rcsY, 0.0f };

			const Vec3 rcsAngle = view - addon;
			auto& toAdd = Vec3{ view - rcsAngle }.normalize().clamp();

			toAdd[0] /= m_pitch->getFloat();
			toAdd[1] /= m_yaw->getFloat();

			Vec2 mouse = Vec2{ *x, *y };
			Vec2 mouseScreen = Vec2{ toAdd[1], -toAdd[0] };

			mouseScreen[0] = (mouseScreen[0] + mouse[0]) / scale;
			mouseScreen[1] = (mouseScreen[1] + mouse[1]) / scale;

			*x = mouseScreen[0];
			*y = mouseScreen[1];
		}

		lastPunch = game::localPlayer->getAimPunch();
	}
	else
	{
		lastPunch = Vec3{ 0.0f, 0.0f, 0.0f };
	}
}

void rcs::run(CUserCmd* cmd)
{
	std::lock_guard lock{ mutex };

	shouldWork = false;

	const auto maybeConfig = configWeapon::get();
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

	const float recoilScale = weapon_recoil_scale->getFloat();
	if (recoilScale <= 0.0f) // recoil doesn't exist, or reversed, then ignore
		return;

	m_cmd = cmd;
	scale = recoilScale;

	shouldWork = true;
}