#include "projectiles.hpp"

#include "nadewarn.hpp"
#include "../../cache/cache.hpp"

#include <SDK/IWeapon.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/renderer/renderer.hpp>

void Projectiles::draw()
{
	if(!game::isAvailable())
		return;

	if (!vars::visuals->world->projectiles->enabled)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::GRENADE_PROJECTILES))
	{
		auto model = entity->getModel();
		if (!model)
			continue;

		auto studio = interfaces::modelInfo->getStudioModel(model);
		if (!studio)
			continue;

		auto wpn = reinterpret_cast<Weapon_t*>(entity);
		if (!wpn) // should not ever happen
			continue;

		auto wpnIdx = game::getNadeByClass(classID, studio);
		if (wpnIdx == WEAPON_NONE)
			continue;

		std::pair<std::string, Color> nades;

		switch (wpnIdx)
		{
		case WEAPON_FLASHBANG:
		{
			nades = { XOR("FLASHBANG"), vars::visuals->world->projectiles->flash() };
			break;
		}
		case WEAPON_HEGRENADE:
		{
			if (wpn->m_nExplodeEffectTickBegin() < 1) // prevent too long time
				nades = { XOR("GRENADE"), vars::visuals->world->projectiles->nade() };
			break;
		}
		case WEAPON_MOLOTOV:
		case WEAPON_INCGRENADE:
		{
			nades = { XOR("FIRE"), vars::visuals->world->projectiles->molotov() };
			break;
		}
		case WEAPON_SMOKEGRENADE:
		{
			if (!reinterpret_cast<Smoke_t*>(wpn)->m_nSmokeEffectTickBegin()) // prevent too long time
				nades = { XOR("SMOKE"), vars::visuals->world->projectiles->smoke() };
			break;
		}
		case WEAPON_DECOY:
		{
			nades = { XOR("DECOY"), vars::visuals->world->projectiles->decoy() }; // this nade time is also too long, check velocity if it's very low
			break;
		}
		case WEAPON_NONE: // understand as NADE_NONE
			return;
		}

		auto [text, col] = nades;

		if (Box box{ entity }; box.isValid())
			imRender.text(box.x + box.w / 2, box.y + box.h + 2, ImFonts::verdana12, text, true, col);
	}
}