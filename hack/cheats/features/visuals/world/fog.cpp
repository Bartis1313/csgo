#include "fog.hpp"

#include "../../cache/cache.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>

#include <cheats/hooks/frameStageNotify.hpp>

namespace
{
	struct FogHandler : hooks::FrameStageNotify
	{
		FogHandler()
		{
			this->registerRun(fogController::run);
			this->registerShutdown(fogController::shutdown);
		}
	} fogHandler;
}

namespace fogController
{
	FogController_t* ent;
}

static constexpr uint32_t U32RGB(const SDKColor& clr)
{
	return (clr.r & 0x0FF) | ((clr.g & 0x0FF) << 8) | ((clr.b & 0x0FF) << 16);
}

void fogController::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	ent = memory::interfaces::fogController();
	if (!ent) // not all maps support it by default
		return;

	if (vars::visuals->world->fog->enabled)
		ent->m_fogenable() = true;
	else
	{
		ent->m_fogenable() = false;
		
		return;
	}

	const SDKColor col = vars::visuals->world->fog->color();

	ent->m_fogstart() = 0.0f;
	ent->m_fogend() = vars::visuals->world->fog->distance * 10.0f;
	ent->m_fogmaxdensity() = col.a / 100.0f;
	ent->m_fogcolorPrimary() = U32RGB(col);
	ent->m_fogcolorSecondary() = U32RGB(col);
}

void fogController::shutdown()
{
	if (!ent)
		return;

	ent->m_fogenable() = false;
}