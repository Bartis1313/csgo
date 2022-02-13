#include "aimbot.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../globals.hpp"
#include "../../../config/vars.hpp"
#include "../../game.hpp"

enum AimbotID
{
    NEAREST = 1,
    HEAD,
    CHEST
};

// because called in paint hook just check if engine in game
// draw simple fov circle that represents aimbot distance
// additionally adds the ratio scale scope
void legitbot::drawFov()
{
    if (interfaces::engine->isConnected() && interfaces::engine->isInGame())
    {
        if (!config.get<bool>(vars.bAimbot))
            return;

        if (!game::localPlayer)
            return;

        if (!game::localPlayer->isAlive())
            return;

        const auto weapon = game::localPlayer->getActiveWeapon();

        if (!weapon)
            return;

        if (weapon->isNonAimable())
            return;

        float radius = std::tan(DEG2RAD(config.get<float>(vars.fFovAimbot)) / 2.0f) / std::tan(DEG2RAD(globals::FOV) / 2.0f) * globals::screenX;

        render::drawCircle(globals::screenX / 2, globals::screenY / 2, radius, 32, Colors::LightBlue);
    }
}

void legitbot::run(CUserCmd* cmd)
{
    if (!config.get<bool>(vars.bAimbot))
        return;

    if (!game::localPlayer)
        return;

    auto weapon = game::localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    if (weapon->isNonAimable())
    {
        // in special cases when weapon that got accepted with aimbot
        // and later on gun got switched to nonaimable, in past snapline was shown, now no
        bestEnt = nullptr;
        return;
    }

    if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
        return;

    if (cmd->m_buttons & IN_ATTACK) // add key later
    {
        float bestFov = config.get<float>(vars.fFovAimbot);
        Vector bestPos = Vector{ 0.0f, 0.0f, 0.0f };
        const auto punch = weapon->isRifle() || weapon->isSmg() ? game::localPlayer->getAimPunch() : Vector(0, 0, 0);
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
                Vector hitPos = Vector{ 0.0f, 0.0f, 0.0f };
                switch (config.get<int>(vars.iAimbot))
                {
                case NEAREST:
                    hitPos = ent->getHitboxPos(pos);
                    break;
                case HEAD:
                    hitPos = ent->getHitboxPos(HITBOX_HEAD);
                    break;
                case CHEST:
                    hitPos = ent->getHitboxPos(HITBOX_LOWER_CHEST);
                    break;
                }
                auto angles = cmd->m_viewangles + punch;
                angles.clamp();

                if (!game::localPlayer->isPossibleToSee(ent, hitPos))
                    continue;

                auto fov = math::calcFov(myEye, hitPos, angles);

                if (fov < bestFov)
                {
                    bestFov = fov;
                    bestPos = hitPos;
                    bestEnt = ent;
                }
                if (config.get<int>(vars.iAimbot) != NEAREST)
                    break;
            }
        }

        if (!bestPos.IsZero())
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
    else
        bestEnt = nullptr;
}

void legitbot::RCS(CUserCmd* cmd)
{
    static Vector oldPunch{ 0.0f, 0.0f, 0.0f };
    auto punch = game::localPlayer->m_aimPunchAngle() * 2.0f;

    punch.x *= config.get<float>(vars.fRCS) / 100.0f;
    punch.y *= config.get<float>(vars.fRCS) / 100.0f;

    auto toMove = cmd->m_viewangles += (oldPunch - punch);
    toMove.clamp();

    interfaces::engine->setViewAngles(toMove);

    oldPunch = punch;
}


void legitbot::runRCS(CUserCmd* cmd)
{
    if (!config.get<bool>(vars.bRCS))
        return;

    if (!game::localPlayer)
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

