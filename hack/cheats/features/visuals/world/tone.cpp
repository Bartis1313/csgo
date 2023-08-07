#include "tone.hpp"

#include "../../cache/cache.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/levelInitPostEnttity.hpp>

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

	struct ToneGrabber : hooks::LevelInitPostEntity
	{
		ToneGrabber()
		{
			this->registerPostReset(toneController::grabberDefault);
		}
	} toneGrabber;
}

namespace toneController
{
	struct Tone
	{
		bool useExposureMin;
		bool useExposureMax;
		bool useBloom;
		float min;
		float max;
		float bloom;
	} toneDefault;
}

void toneController::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	auto ent = memory::interfaces::toneController();
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

void toneController::grabberDefault()
{
	auto ent = memory::interfaces::toneController();
	if (!ent)
		return;

	toneDefault.useExposureMin = ent->m_bUseCustomAutoExposureMin();
	toneDefault.useExposureMax = ent->m_bUseCustomAutoExposureMax();
	toneDefault.useBloom = ent->m_bUseCustomBloomScale();
	toneDefault.min = ent->m_flCustomAutoExposureMin();
	toneDefault.max = ent->m_flCustomAutoExposureMax();
	toneDefault.bloom = ent->m_flCustomBloomScale();
}

void toneController::shutdown()
{
	auto ent = memory::interfaces::toneController();
	if (!ent)
		return;

	ent->m_bUseCustomAutoExposureMin() = toneDefault.useExposureMin;
	ent->m_bUseCustomAutoExposureMax() = toneDefault.useExposureMax;
	ent->m_bUseCustomBloomScale() = toneDefault.useBloom;
	ent->m_flCustomAutoExposureMin() = toneDefault.min;
	ent->m_flCustomAutoExposureMax() = toneDefault.max;
	ent->m_flCustomBloomScale() = toneDefault.bloom;
}