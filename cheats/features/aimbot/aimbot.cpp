#include "aimbot.hpp"

#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/vars.hpp"
#include "../../../SDK/Enums.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../config/vars.hpp"
#include "../../globals.hpp"
#include "../../game.hpp"

#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"

void Aimbot::run(CUserCmd* cmd)
{
    if (!config.get<bool>(vars.bAimbot))
        return;

    if (!game::isAvailable())
        return;

    if (!game::localPlayer->isAlive())
        return;

    auto weapon = game::localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    if (weapon->isNonAimable())
    {
        resetFields(); // switching to knife when clicked
        return;
    }

    if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
        return;

    const auto myEye = game::localPlayer->getEyePos();
    const auto punch = (weapon->isRifle() || weapon->isSmg()) ? game::localPlayer->getAimPunch() : Vector{};

    if (!isClicked(cmd))
    {
        resetFields();
        return;
    }

    if (!getBestTarget(cmd, weapon, myEye, punch))
        return;

    if (m_bestHitpos.isZero())
        return;
   
    auto angle = math::calcAngleRelative(myEye, m_bestHitpos, cmd->m_viewangles + punch);
    angle.clamp();

    if (auto smoothing = config.get<float>(vars.fSmooth); smoothing)
        angle /= smoothing;
    cmd->m_viewangles += angle;

    interfaces::engine->setViewAngles(cmd->m_viewangles);
}

void Aimbot::RCS(CUserCmd* cmd)
{
    static Vector oldPunch = {};
    const static auto scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"))->getFloat();
    auto punch = game::localPlayer->m_aimPunchAngle() * scale;

    punch.x *= config.get<float>(vars.fRCSx) / 100.0f;
    punch.y *= config.get<float>(vars.fRCSy) / 100.0f;

    auto toMove = cmd->m_viewangles += (oldPunch - punch);
    toMove.clamp();

    interfaces::engine->setViewAngles(toMove);

    oldPunch = punch;
}

void Aimbot::runRCS(CUserCmd* cmd)
{
    if (!config.get<bool>(vars.bRCS))
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

    RCS(cmd);
}

//#include "../../../utilities/console/console.hpp"

bool Aimbot::getBestTarget(CUserCmd* cmd, Weapon_t* wpn, const Vector& eye, const Vector& punch)
{ 
    /*bool isSame = m_temp == m_bestEnt;
    console.print("{}\n", isSame);*/

    static float delay = 0.0f;
    // will not work for the special case:
    // delay did not hit timer limit but we switched manually to new target -> should reset the timer. I couldn't detect it without false positives :(
    // epic solution - stop shooting and start again
    if (config.get<bool>(vars.bAimbotDelay))
    {
        /*if (m_bestEnt && m_bestEnt->isAlive())
        {
            m_temp = m_bestEnt;
        }*/
        if (m_bestEnt && !m_delay && !m_bestEnt->isAlive()) // if ent is found and dead, then set field to delay and wait curr time + cfgtime
        {
            m_delay = true;
            delay = interfaces::globalVars->m_curtime + (config.get<float>(vars.fAimbotDelay) / 1000.0f);
        }
        if (m_delay) // if the delay is hit, check time, so when ent died
        {
            if (delay <= interfaces::globalVars->m_curtime)
                m_delay = false;
            else // need reset fields here and stop the method
            {
                m_bestHitpos = {};
                m_bestEnt = nullptr;
                return false;
            }
        }
    }

    m_bestEnt = nullptr;
    m_bestHitpos = {};

    auto hitboxes = getHitboxes();

    float bestFov = config.get<float>(vars.fFovAimbot);

    for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
    {
        auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

        if (!ent)
            continue;

        if (ent == game::localPlayer)
            continue;

        if (!ent->isAlive() || !game::localPlayer->isAlive())
            continue;

        if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
            continue;

        if (ent->isDormant())
            continue;

        if (ent->m_bGunGameImmunity())
            continue;

        for (const auto pos : hitboxes)
        {
            Vector hitPos = ent->getHitboxPos(pos);

            auto angles = cmd->m_viewangles + punch;
            angles.clamp();

            if (!game::localPlayer->isPossibleToSee(hitPos))
                continue;

            auto fov = math::calcFov(eye, hitPos, angles);

            if (fov < bestFov)
            {
                bestFov = fov;
                m_bestEnt = ent;
                m_bestHitpos = hitPos;
            }
        }
    }
    return m_bestEnt ? true : false;
}

Player_t* Aimbot::getTargetted()
{
    return m_bestEnt;
}

void Aimbot::resetFields()
{
    m_bestEnt = nullptr;
    m_bestHitpos = {};
    m_delay = false;
}

void Aimbot::drawBestPoint()
{
    if (!config.get<bool>(vars.bDrawBestPoint))
        return;

    if (!game::isAvailable())
        return;

    if (m_bestHitpos.isZero())
        return;

    if (Vector2D p; imRender.worldToScreen(m_bestHitpos, p))
        imRender.drawCircleFilled(p.x, p.y, 5, 12, Colors::Cyan);
}

void Aimbot::drawFov()
{
    if (!config.get<bool>(vars.bDrawFov))
        return;

    if (!config.get<bool>(vars.bAimbot))
        return;

    if (!config.get<float>(vars.fFovAimbot))
        return;

    if (!game::isAvailable())
        return;

    const auto weapon = game::localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    if (weapon->isNonAimable())
        return;

    float radius = std::tan(DEG2RAD(config.get<float>(vars.fFovAimbot)) / 2.0f) / std::tan(DEG2RAD(globals::FOV) / 2.0f) * globals::screenX;

    imRender.drawCircle(globals::screenX / 2.0f, globals::screenY / 2.0f, radius, 32, config.get<Color>(vars.cDrawFov));
}

bool Aimbot::isClicked(CUserCmd* cmd)
{
    if (config.get<bool>(vars.bAimbotUseKey))
    {
        if (config.get<Key>(vars.kAimbotKey).isEnabled())
            return true;
        else
            return false;
    }
    else
        return cmd->m_buttons & IN_ATTACK;
}

std::vector<size_t> Aimbot::getHitboxes()
{
    std::vector<size_t> hitboxes = {};

    switch (config.get<int>(vars.iAimbot))
    {
    case E2T(AimbotID::NEAREST):
    {
        for (size_t i = HITBOX_HEAD; i < HITBOX_MAX; i++)
            hitboxes.push_back(i);

        break;
    }
    case E2T(AimbotID::HEAD):
        hitboxes = { HITBOX_HEAD };
        break;
    case E2T(AimbotID::CHEST):
        hitboxes = { HITBOX_LOWER_CHEST };
        break;
    default:
        break;
    }

    return hitboxes;
}

void Aimbot::draw()
{
    drawFov();
    drawBestPoint();
}
