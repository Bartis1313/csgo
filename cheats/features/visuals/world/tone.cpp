#include "tone.hpp"

#include "../../cache/cache.hpp"

#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>

void ToneController::init()
{

}

void ToneController::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::CONTROLLERS))
	{
		if (classID != CEnvTonemapController)
			continue;

		auto ent = reinterpret_cast<EnvTonemapController_t*>(entity);

		// this is needed to ONLY change when we changed anything
		if (auto cfg = config.get<bool>(vars.bControlTone); cfg && (m_checkStateSlider || m_checkStateButton))
		{
			ent->m_bUseCustomAutoExposureMin() = cfg;
			ent->m_bUseCustomAutoExposureMax() = cfg;
			ent->m_bUseCustomBloomScale() = cfg;
			ent->m_flCustomAutoExposureMin() = config.get<float>(vars.fControlToneMin);
			ent->m_flCustomAutoExposureMax() = config.get<float>(vars.fControlToneMax);
			ent->m_flCustomBloomScale() = config.get<float>(vars.fControlToneBloomScale);
		}
		else if (globals::isShutdown || (!cfg && m_checkStateButton))
		{
			ent->m_bUseCustomAutoExposureMin() = false;
			ent->m_bUseCustomAutoExposureMax() = false;
			ent->m_bUseCustomBloomScale() = false;
		}
	}
}
