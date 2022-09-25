#include "smoke.hpp"

#include "../../cache/cache.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/math/math.hpp>
#include <gamememory/memory.hpp>

void SmokeDraw::init()
{

}

void SmokeDraw::draw()
{
	if (!config.get<bool>(vars.bDrawSmoke))
		return;

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::GRENADE_PROJECTILES))
	{
		if (classID != CSmokeGrenadeProjectile)
			continue;

		auto ent = reinterpret_cast<Smoke_t*>(entity);

		if (!ent->m_nSmokeEffectTickBegin())
		{
			drawCustomSmokeEffect(ent->absOrigin(), 30.0f);
			continue;
		}

		float time = game::ticksToTime(ent->m_nSmokeEffectTickBegin()) + ent->expireTime() - interfaces::globalVars->m_curtime;
		float scale = time / ent->expireTime();

		const auto& origin = ent->absOrigin();
		constexpr int smokeRadius = 144;

		imRender.drawCircle3DFilled(origin, smokeRadius, 216, Colors::Grey, Colors::Black, true, 2.0f);
		drawCustomSmokeEffect(origin, smokeRadius);

		static float size = ImFonts::tahoma14->FontSize;
		if (ImVec2 s; imRender.worldToScreen(origin, s))
		{
			imRender.drawProgressRing(s.x, s.y, 25, 32, -90, scale, 5.0f, Colors::LightBlue);
			imRender.text(s.x, s.y - (size / 2.0f), ImFonts::tahoma14, FORMAT(XOR("{:.2f}s"), time), true, Colors::White);
		}
	}
}

void SmokeDraw::drawCustomSmokeEffect(const Vector& pos, float radius)
{
	// clockwise for better effect
	Vector end = { radius * std::sin(interfaces::globalVars->m_curtime) + pos.x, radius * std::cos(interfaces::globalVars->m_curtime) + pos.y, pos.z };

	interfaces::effects->smoke(end, -1, 5.0f, 1.0f);
}

void SmokeRemoval::init()
{
	
}

void SmokeRemoval::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::localPlayer)
		return;

	if (config.get<bool>(vars.bEditEffectsSmoke)) // remove effects from inside, this is why we nulling smoke count
		*reinterpret_cast<uintptr_t*>(g_Memory.m_smokeCount()) = 0;
}
