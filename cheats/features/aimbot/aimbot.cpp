#include "aimbot.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../globals.hpp"


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
        if (!vars::iAimbot)
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

        int x, y;
        interfaces::engine->getScreenSize(x, y);

        float radius = std::tan(DEG2RAD(vars::iFovAimbot) / 2.0f) / std::tan(DEG2RAD(globals::FOV) / 2.0f) * x;

        render::drawCircle(x / 2, y / 2, radius, 32, Colors::LightBlue);
    }
}

inline constexpr std::vector<int> posToAim()
{
    std::vector<int> vec = {};
    switch (vars::iAimbot)
    {
    case NEAREST:
        for (int i = 0; i < HITBOX_MAX; i++)
            vec.emplace_back(i);
        return vec;
    case HEAD:
        return { HITBOX_HEAD };
    case CHEST:
        return { HITBOX_LOWER_CHEST };
    default:
        return {};
    }
    return {};
}


Vector legitbot::getBestBonePos(CUserCmd* cmd)
{
    float bestFov = vars::iFovAimbot;
    //int unseenHitboxes = 0;
    Vector bestHitbox = { 0, 0, 0 };

    const auto weapon = game::localPlayer->getActiveWeapon();
    const auto& punch = weapon->isRifle() ? game::localPlayer->getAimPunch() : Vector(0, 0, 0);
    // TODO: test vfunc for performance later
    const auto myEye = game::localPlayer->getEyePos();

    for (int i = 1; i < interfaces::engine->getMaxClients(); i++)
    {
        const auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

        if (!ent)
            continue;

        if (!ent->isAlive() || !game::localPlayer->isAlive())
            continue;

        if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
            continue;

        if (ent->isDormant())
            continue;

        if (ent->m_bGunGameImmunity())
            continue;

        for (const auto& pos : posToAim())
        {          
            auto hitbox = ent->getHitboxPos(pos);

            auto angles = cmd->m_viewangles + punch;

            auto fov = math::calcFov(myEye, hitbox, angles);

            if (fov < bestFov)
            {
                bestFov = fov;
                bestHitbox = hitbox;
                bestEnt = ent;
            }
            // will not hit this though, since loop has always valid ranged size
            if (vars::iAimbot != NEAREST)
                break;
        }
    }
    bestPoint = bestHitbox;
    return bestHitbox;
}

void legitbot::RCS(CUserCmd* cmd)
{
    static Vector oldPunch{ 0, 0, 0 };
    auto punch = game::localPlayer->m_aimPunchAngle() * 2.0f;

    punch.x *= vars::iRCS / 100.0f;
    punch.y *= vars::iRCS / 100.0f;

    auto& toMove = cmd->m_viewangles += (oldPunch - punch);
    toMove.clamp();

    interfaces::engine->setViewAngles(toMove);

    oldPunch = punch;
}

void legitbot::run(CUserCmd* cmd)
{
    if (!vars::iAimbot && !vars::iRCS)
        return;

    if (!game::localPlayer)
        return;

    if (!interfaces::engine->isConnected() || !interfaces::engine->isInGame())
        return;

    if (!game::localPlayer->isAlive())
        return;

    auto weapon = game::localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    if (weapon->isNonAimable())
        return;

    if (weapon->isSniper() && !game::localPlayer->m_bIsScoped())
        return;

    if (vars::iRCS)
        RCS(cmd);

    auto punch = weapon->isRifle() ? game::localPlayer->getAimPunch() : Vector(0, 0, 0);
    auto myEye = game::localPlayer->getEyePos();

    if (cmd->m_buttons & IN_ATTACK && vars::iAimbot) // add key later
    {
        const auto bestPos = getBestBonePos(cmd);

        if (!bestPos.IsZero() && bestEnt)
        {
            auto angle = math::calcAngleRelative(myEye, bestPos, cmd->m_viewangles + punch);

            angle.clamp();

            // remember to only smooth when the value is non zero/negative!
            if (vars::iSmooth)
                angle /= vars::iSmooth;
            cmd->m_viewangles += angle;

            interfaces::engine->setViewAngles(cmd->m_viewangles);
        }
    }
}
