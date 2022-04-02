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

Player_t* Aimbot::getBest() const
{
    return m_bestEnt;
}

void Aimbot::resetBests()
{
    m_bestEnt = nullptr;
    m_bestHitpos = {};
}

void Aimbot::drawBestPoint()
{
    if (!config.get<bool>(vars.bDrawBestPoint))
        return;

    if (!game::isAvailable())
        return;

    if (!m_bestEnt)
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

void Aimbot::run(CUserCmd* cmd)
{
    if (!config.get<bool>(vars.bAimbot))
        return;

    if (!game::localPlayer)
        return;

    resetBests();

    auto weapon = game::localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    if (weapon->isNonAimable())
        return;

    if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
        return;

    if (cmd->m_buttons & IN_ATTACK) // add key later
    {
        float bestFov = config.get<float>(vars.fFovAimbot);
        Vector bestPos = {};
        const auto punch = (weapon->isRifle() || weapon->isSmg()) ? game::localPlayer->getAimPunch() : Vector{};
        const auto myEye = game::localPlayer->getEyePos();

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
            
            // vector can be used to... to detect hotboxes, after gui is done you can easily make vector with boolens as multibox for them
            // if you want bones which I dislike because it will be useless effort, some bone ids are not the same for every model
            // https://cdn.discordapp.com/attachments/829304849027170305/916477147261051000/unknown.png
            for (int pos = HITBOX_HEAD; pos < HITBOX_MAX; pos++)
            {
                Vector hitPos = {};
                switch (config.get<int>(vars.iAimbot))
                {
                case E2T(AimbotID::NEAREST):
                    hitPos = ent->getHitboxPos(pos);
                    break;
                case E2T(AimbotID::HEAD):
                    hitPos = ent->getHitboxPos(HITBOX_HEAD);
                    break;
                case E2T(AimbotID::CHEST):
                    hitPos = ent->getHitboxPos(HITBOX_LOWER_CHEST);
                    break;
                default:
                    break;
                }
                auto angles = cmd->m_viewangles + punch;
                angles.clamp();

                if (!game::localPlayer->isPossibleToSee(hitPos))
                    continue;

                auto fov = math::calcFov(myEye, hitPos, angles);

                if (fov < bestFov)
                {
                    bestFov = fov;
                    bestPos = hitPos;
                    m_bestEnt = ent;
                    m_bestHitpos = hitPos;
                }
                if (config.get<int>(vars.iAimbot) != E2T(AimbotID::NEAREST))
                    break;
            }
        }

        if (!bestPos.isZero())
        {
            auto angle = math::calcAngleRelative(myEye, bestPos, cmd->m_viewangles + punch);
            angle.clamp();

            // remember to only smooth when the value is non zero/negative!
            if (auto smoothing = config.get<float>(vars.fSmooth); smoothing)
                angle /= smoothing;
            cmd->m_viewangles += angle;

            interfaces::engine->setViewAngles(cmd->m_viewangles);
        }
    }
}

void Aimbot::RCS(CUserCmd* cmd)
{
    static Vector oldPunch = {};
    const static auto scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"))->getFloat();
    auto punch = game::localPlayer->m_aimPunchAngle() * scale;

    punch.x *= config.get<float>(vars.fRCS) / 100.0f;
    punch.y *= config.get<float>(vars.fRCS) / 100.0f;

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

