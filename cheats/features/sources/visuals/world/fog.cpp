#include "fog.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"

static constexpr uint32_t U32RGB(const SDKColor& clr)
{
	return (clr.r & 0x0FF) | ((clr.g & 0x0FF) << 8) | ((clr.b & 0x0FF) << 16);
}

void FogController::run(FogController_t* ent)
{
	if (!ent)
		return;

	if (!game::isAvailable())
		return;

	if (globals::isShutdown)
	{
		ent->m_fogenable() = false;
		return;
	}

	if (bool opt = config.get<bool>(vars.bFog); opt)
		ent->m_fogenable() = opt;
	else
	{
		ent->m_fogenable() = opt;
		return;
	}

	SDKColor col = config.get<CfgColor>(vars.cFog).getColor();

	ent->m_fogstart() = 0.0f;
	ent->m_fogend() = config.get<float>(vars.fFogDistance) * 10.0f;
	ent->m_fogmaxdensity() = col.a / 100.0f;
	ent->m_fogcolorPrimary() = U32RGB(col);
	ent->m_fogcolorSecondary() = U32RGB(col);
}
