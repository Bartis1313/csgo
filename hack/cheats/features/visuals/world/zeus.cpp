#include "zeus.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/game.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <render/render.hpp>

void ZeusDraw::init()
{
	m_party = memory::interfaces::cvar->findVar("sv_party_mode");
}

void ZeusDraw::draw()
{
	if (!game::isAvailable())
		return;

	vars::visuals->world->zeus->party ? m_party->setValue(true) : m_party->setValue(false);

	if (!vars::visuals->world->zeus->enabled)
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_TASER)
	{
		const static float range = weapon->getWpnInfo()->m_range;
		const Vec3 abs = game::localPlayer->absOrigin() + Vec3{ 0.0f, 0.0f, 30.0f }; // small correction to get correct trace visually, will still throw false positives on stairs etc...

		Color color = vars::visuals->world->zeus->color();

		if (vars::visuals->world->zeus->tracing)
			imRender.drawCircle3DTraced(abs, range, 32, game::localPlayer(), color, true, 2.5f);
		else
			imRender.drawCircle3D(abs, range, 32, color, true, 2.0f);
	}
}