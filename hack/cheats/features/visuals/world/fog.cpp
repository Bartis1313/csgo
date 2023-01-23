#include "fog.hpp"

#include "../../cache/cache.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>

static constexpr uint32_t U32RGB(const SDKColor& clr)
{
	return (clr.r & 0x0FF) | ((clr.g & 0x0FF) << 8) | ((clr.b & 0x0FF) << 16);
}

void FogController::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::CONTROLLERS))
	{
		if (classID != CFogController)
			continue;

		auto ent = reinterpret_cast<FogController_t*>(entity);

		if (globals::isShutdown)
		{
			ent->m_fogenable() = false;
			break;
		}

		if (bool opt = vars::visuals->world->fog->enabled; opt)
			ent->m_fogenable() = opt;
		else
		{
			ent->m_fogenable() = opt;
			break;
		}

		SDKColor col = vars::visuals->world->fog->color();

		ent->m_fogstart() = 0.0f;
		ent->m_fogend() = vars::visuals->world->fog->distance * 10.0f;
		ent->m_fogmaxdensity() = col.a / 100.0f;
		ent->m_fogcolorPrimary() = U32RGB(col);
		ent->m_fogcolorSecondary() = U32RGB(col);
	}
}
