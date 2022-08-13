#include "aimbot.hpp"

#include "../../../../SDK/IVEngineClient.hpp"
#include "../../../../SDK/CUserCmd.hpp"
#include "../../../../SDK/CGlobalVars.hpp"
#include "../../../../SDK/IClientEntityList.hpp"
#include "../../../../SDK/vars.hpp"
#include "../../../../SDK/Enums.hpp"
#include "../../../../SDK/ICvar.hpp"
#include "../../../../SDK/ConVar.hpp"
#include "../../../../SDK/IVModelInfo.hpp"
#include "../../../../SDK/structs/Entity.hpp"
#include "../../../../SDK/math/Vector.hpp"
#include "../../../../SDK/interfaces/interfaces.hpp"

#include "../../../config/vars.hpp"
#include "../../../globals.hpp"
#include "../../../game.hpp"
#include "../../../utilities/math/math.hpp"
#include "../backtrack/backtrack.hpp"

void Aimbot::init()
{
    
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
    m_config = config.get<std::vector<CfgWeapon>>(vars.arrAimbot).at(indexCfg);

    if (!m_config.m_aimEnabled)
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

    Vector m_posToAim = m_bestHitpos;
    if (m_config.m_aimbacktrack)
    {
        int boneID = 8; // HEAD start
        if (auto modelStudio = interfaces::modelInfo->getStudioModel(m_bestEnt->getModel()); modelStudio != nullptr)
        {
            if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(m_bestId); hitbox != nullptr)
            {
                boneID = hitbox->m_bone;
            }
        }
        auto record = g_Backtrack.getAllRecords().at(m_bestEnt->getIndex());
        m_posToAim = record.at(record.size() / 2).m_matrix[boneID].origin(); // middle, u can play with this as u want to
    }
   
    auto angle = math::calcAngleRelative(myEye, m_posToAim, cmd->m_viewangles + punch);
    angle.clamp();

    Vector delta = angle + cmd->m_viewangles - m_view;

    if (auto smoothing = m_config.m_smooth; smoothing)
        angle /= smoothing;

    cmd->m_viewangles += angle;

    interfaces::engine->setViewAngles(cmd->m_viewangles);
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
    if (m_config.m_aimDelayEnabled)
    {
        /*if (m_bestEnt && m_bestEnt->isAlive())
        {
            m_temp = m_bestEnt;
        }*/
        if (m_bestEnt && !m_delay && !m_bestEnt->isAlive()) // if ent is found and dead, then set field to delay and wait curr time + cfgtime
        {
            m_delay = true;
            delay = interfaces::globalVars->m_curtime + (m_config.m_aimDelay / 1000.0f);
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

    float bestFov = m_config.m_fov;

    if (game::localPlayer->m_flFlashDuration() > 0.0f)
    {
        if (game::localPlayer->m_flFlashBangTime() >= m_config.m_flashAlphaLimit)
            return false;
    }

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

            if (!game::localPlayer->isPossibleToSee(ent, hitPos))
                continue;

            if (m_config.m_smokeCheck && game::localPlayer->isViewInSmoke(hitPos))
                continue;

            auto fov = m_config.m_methodAim == E2T(AimbotMethod::CROSSHAIR)
                ? math::calcFov(eye, hitPos, angles)
                : math::calcFovReal(eye, hitPos, angles);

            if (fov < bestFov)
            {
                bestFov = fov;
                m_bestId = pos;
                m_bestEnt = ent;
                m_bestHitpos = hitPos;
            }
        }
    }
    return m_bestEnt ? true : false;
}

Player_t* Aimbot::getTargetted() const
{
    return m_bestEnt;
}

Vector Aimbot::getCachedView() const
{
    return m_view;
}

Vector Aimbot::getBestHibox() const
{
    return m_bestHitpos;
}

CfgWeapon Aimbot::getCachedConfig() const
{
    return m_config;
}

void Aimbot::resetFields()
{
    m_bestEnt = nullptr;
    m_bestHitpos = {};
    m_delay = false;
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

    switch (m_config.m_aimSelection)
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
