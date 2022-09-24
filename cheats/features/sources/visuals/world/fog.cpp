#include "fog.hpp"

#include "../../cache/cache.hpp"

#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>

void FogController::init()
{

}

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

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::CONTROLLERS))
	{
		if (classID != CFogController)
			continue;

		auto ent = reinterpret_cast<FogController_t*>(entity);

		if (globals::isShutdown)
		{
			ent->m_fogenable() = false;
			break;
		}

		if (bool opt = config.get<bool>(vars.bFog); opt)
			ent->m_fogenable() = opt;
		else
		{
			ent->m_fogenable() = opt;
			break;
		}

		SDKColor col = config.get<CfgColor>(vars.cFog).getColor();

		ent->m_fogstart() = 0.0f;
		ent->m_fogend() = config.get<float>(vars.fFogDistance) * 10.0f;
		ent->m_fogmaxdensity() = col.a / 100.0f;
		ent->m_fogcolorPrimary() = U32RGB(col);
		ent->m_fogcolorSecondary() = U32RGB(col);
	}
}
