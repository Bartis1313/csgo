#include "zeus.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <utilities/renderer/renderer.hpp>

void ZeusDraw::init()
{
	m_party = interfaces::cvar->findVar(XOR("sv_party_mode"));
}

void ZeusDraw::draw()
{
	if (!game::isAvailable())
		return;

	config.get<bool>(vars.bZeusPartyMode) ? m_party->setValue(true) : m_party->setValue(false);

	if (!config.get<bool>(vars.bDrawZeusRange))
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_TASER)
	{
		const static float range = weapon->getWpnInfo()->m_range;
		const Vector abs = game::localPlayer->absOrigin() + Vector(0.0f, 0.0f, 30.0f); // small correction to get correct trace visually, will still throw false positives on stairs etc...

		CfgColor color = config.get<CfgColor>(vars.cZeusRange);

		if (config.get<bool>(vars.bZeusUseTracing))
			imRender.drawCircle3DTraced(abs, range, 32, game::localPlayer(), color.getColor(), true, 2.5f);
		else
			imRender.drawCircle3D(abs, range, 32, color.getColor(), true, 2.0f);
	}
}