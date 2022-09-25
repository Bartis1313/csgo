#include "RCS.hpp"

#include "../aimbot/aimbot.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

#include <game/game.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

void RCS::init()
{
    m_scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"));
}

void RCS::run(CUserCmd* cmd)
{
    auto cfg = g_Aimbot.getCachedConfig();

    if (!cfg.m_RcsEnabled)
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

    prepare(cmd);
}

void RCS::prepare(CUserCmd* cmd)
{
    auto cfg = g_Aimbot.getCachedConfig();

    static Vector oldPunch = game::localPlayer->m_aimPunchAngle() * m_scale->getFloat();
    if (cmd->m_buttons & IN_ATTACK)
    {
        Vector punch = game::localPlayer->m_aimPunchAngle() * m_scale->getFloat();

        punch.x *= cfg.m_RcsX / 100.0f;
        punch.y *= cfg.m_RcsY / 100.0f;

        Vector toMove = cmd->m_viewangles += (oldPunch - punch);
        toMove.clamp();

        interfaces::engine->setViewAngles(toMove);

        oldPunch = punch;
    }
    else
    {
        Vector punch = game::localPlayer->m_aimPunchAngle() * m_scale->getFloat();
        punch.x *= cfg.m_RcsX / 100.0f;
        punch.y *= cfg.m_RcsY / 100.0f;

        oldPunch = punch;
    }
}
