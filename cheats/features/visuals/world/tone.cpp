#include "tone.hpp"

#include "../../cache/cache.hpp"

#include <game/game.hpp>
#include <game/globals.hpp>
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
		if (auto cfg = vars::visuals->world->tone->enabled; cfg && (m_checkStateSlider || m_checkStateButton))
		{
			ent->m_bUseCustomAutoExposureMin() = cfg;
			ent->m_bUseCustomAutoExposureMax() = cfg;
			ent->m_bUseCustomBloomScale() = cfg;
			ent->m_flCustomAutoExposureMin() = vars::visuals->world->tone->min;
			ent->m_flCustomAutoExposureMax() = vars::visuals->world->tone->max;
			ent->m_flCustomBloomScale() = vars::visuals->world->tone->bloom;
		}
		else if (globals::isShutdown || (!cfg && m_checkStateButton))
		{
			ent->m_bUseCustomAutoExposureMin() = false;
			ent->m_bUseCustomAutoExposureMax() = false;
			ent->m_bUseCustomBloomScale() = false;
		}
	}
}
