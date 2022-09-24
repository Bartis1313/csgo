#include "aimbotDraw.hpp"

#include <SDK/IWeapon.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/wrappers.hpp>

#include "aimbot.hpp"

void AimbotDraw::init()
{

}

void AimbotDraw::draw()
{
    drawFov();
    drawBestPoint();
}

void AimbotDraw::drawFov()
{
    auto cfg = g_Aimbot.getCachedConfig();

    if (!config.get<bool>(vars.bDrawFov))
        return;

    if (!cfg.m_aimEnabled)
        return;

    if (!cfg.m_fov)
        return;

    if (!game::isAvailable())
        return;

    const auto weapon = game::localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    if (weapon->isNonAimable())
        return;

    float radius = 0.0f;

    switch (cfg.m_methodAim)
    {
    case E2T(AimbotMethod::CROSSHAIR):
    {
        radius = std::tan(math::DEG2RAD(cfg.m_fov) / 2.0f) / std::tan(math::DEG2RAD(globals::FOV) / 2.0f) * globals::screenX;

        break;
    }
    case E2T(AimbotMethod::DYNAMIC): // not very very accurate
    {
        Trace_t trace;
        TraceFilter filter{ game::localPlayer() };
        const auto myEye = game::localPlayer->getEyePos();
        constexpr float range = 8192.0f; // because we need max range possible
        const auto view = g_Aimbot.getCachedView();
        const auto end = myEye + (math::angleVec(view) * range);
        interfaces::trace->traceRay({ myEye, end }, MASK_SHOT, &filter, &trace);
        // 3d end
        const auto destination = trace.m_end;

        // turn for visualization
        auto forward = math::angleVec({ view.x, view.y + 90.f, 0.f });
        // dist in calcFovReal
        forward *= cfg.m_fov * 10.0f;
        // final vector where we aim
        auto aimingView = destination + forward;

        if (ImVec2 v; imRender.worldToScreen(aimingView, v))
            radius = std::abs(globals::screenX / 2.0f - v.x);

        break;
    }
    }

    imRender.drawCircle(globals::screenX / 2.0f, globals::screenY / 2.0f, radius, 32, config.get<CfgColor>(vars.cDrawFov).getColor());
}

void AimbotDraw::drawBestPoint()
{
    if (!config.get<bool>(vars.bDrawBestPoint))
        return;

    if (!game::isAvailable())
        return;

    const auto hitbox = g_Aimbot.getBestHibox();
    if (hitbox.isZero())
        return;

    if (Vector2D p; imRender.worldToScreen(hitbox, p))
        imRender.drawCircleFilled(p.x, p.y, 5, 12, Colors::Cyan);
}
