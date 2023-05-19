#include "tone.hpp"

#include "../../cache/cache.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>

#include <cheats/hooks/frameStageNotify.hpp>

namespace
{
	struct ToneHandler : hooks::FrameStageNotify
	{
		ToneHandler()
		{
			this->registerRun(toneController::run);
			this->registerShutdown(toneController::shutdown);
		}
	} toneHandler;
}

namespace toneController
{
	EnvTonemapController_t* ent{ };
}

void toneController::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	ent = memory::interfaces::toneController();
	if (!ent)
		return;

	const bool isOn = ent->m_bUseCustomAutoExposureMin() || ent->m_bUseCustomAutoExposureMax();

	if (!ent) // not all maps support it by default
		return;

	if (bool cfg = vars::visuals->world->tone->enabled; cfg)
	{
		ent->m_bUseCustomAutoExposureMin() = true;
		ent->m_bUseCustomAutoExposureMax() = true;
		ent->m_bUseCustomBloomScale() = true;
		ent->m_flCustomAutoExposureMin() = vars::visuals->world->tone->min;
		ent->m_flCustomAutoExposureMax() = vars::visuals->world->tone->max;
		ent->m_flCustomBloomScale() = vars::visuals->world->tone->bloom;
	}
	else if (!cfg)
	{
		shutdown();
	}
}

void toneController::shutdown()
{
	if (!ent)
		return;

	ent->m_bUseCustomAutoExposureMin() = false;
	ent->m_bUseCustomAutoExposureMax() = false;
	ent->m_bUseCustomBloomScale() = false;
	ent->m_flCustomAutoExposureMin() = 0.0f;
	ent->m_flCustomAutoExposureMax() = 0.0f;
	ent->m_flCustomBloomScale() = 0.0f;
}