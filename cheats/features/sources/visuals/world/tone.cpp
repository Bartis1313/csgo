#include "tone.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"

void ToneController::run(EnvTonemapController_t* ent)
{
	if (!game::isAvailable())
		return;

	// this is needed to ONLY change when we changed anything
	if (auto cfg = config.get<bool>(vars.bControlTone); cfg && (m_checkStateSlider || m_checkStateButton))
	{
		ent->m_bUseCustomAutoExposureMin() = cfg;
		ent->m_bUseCustomAutoExposureMax() = cfg;
		ent->m_flCustomAutoExposureMin() = config.get<float>(vars.fControlToneMin);
		ent->m_flCustomAutoExposureMax() = config.get<float>(vars.fControlToneMax);
	}
	else if (globals::isShutdown || (!cfg && m_checkStateButton))
	{
		ent->m_bUseCustomAutoExposureMin() = false;
		ent->m_bUseCustomAutoExposureMax() = false;
	}
}
