#include "molotov.hpp"

#include "../../cache/cache.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

void MolotovDraw::draw()
{
	if (!vars::visuals->world->molotov->enabled)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::GRENADE_PROJECTILES))
	{
		if (classID != CInferno)
			continue;

		auto molotov = reinterpret_cast<Inferno_t*>(entity);

		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/cstrike15/Effects/inferno.cpp
		// here you can see ratios and everything

		float time = molotov->spawnTime() + molotov->expireTime() - memory::interfaces::globalVars->m_curtime;
		float scale = time / molotov->expireTime();

		const auto& origin = molotov->absOrigin();
		constexpr int molotovRadius = 60; // 30 * 2

		Color col = vars::visuals->world->molotov->color();

		for (int i = 0; i < molotov->m_fireCount(); i++)
		{
			auto pos = origin + molotov->getInfernoPos(i);
			imRender.drawCircle3DFilled(pos, molotovRadius, 32, col, col);
		}
		static float size = ImFonts::tahoma14->FontSize;
		// timer
		if (ImVec2 s; imRender.worldToScreen(origin, s))
		{
			imRender.drawProgressRing(s.x, s.y, 25, 32, -90.0f, scale, 5.0f, Colors::LightBlue);
			imRender.text(s.x, s.y - (size / 2.0f), ImFonts::tahoma14, std::format("{:.2f}s", time), true, Colors::White);
		}
	}
}