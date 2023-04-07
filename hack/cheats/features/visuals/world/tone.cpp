#include "tone.hpp"

#include "../../cache/cache.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>

void ToneController::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::CONTROLLERS))
	{
		if (classID != CEnvTonemapController)
			continue;

		auto ent = reinterpret_cast<EnvTonemapController_t*>(entity);

		// this is needed to ONLY change when we changed anything
		// this is also stupid, CEnvTonemapController adr changes only on map change (not tested), but I guess so
		// and overwriting those need just check the memory instead of those stupid menu checks
		// remove later, TODO
		if (auto cfg = vars::visuals->world->tone->enabled; cfg && (m_checkStateSlider || m_checkStateButton))
		{
			ent->m_bUseCustomAutoExposureMin() = true;
			ent->m_bUseCustomAutoExposureMax() = true;
			ent->m_bUseCustomBloomScale() = true;
			ent->m_flCustomAutoExposureMin() = vars::visuals->world->tone->min;
			ent->m_flCustomAutoExposureMax() = vars::visuals->world->tone->max;
			ent->m_flCustomBloomScale() = vars::visuals->world->tone->bloom;
		}
		if (globals::isShutdown)
		{
			ent->m_bUseCustomAutoExposureMin() = false;
			ent->m_bUseCustomAutoExposureMax() = false;
			ent->m_bUseCustomBloomScale() = false;
		}
	}
}
