#include "cache.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <game/game.hpp>

#include <ranges>

void EntityCache::init()
{

}

void EntityCache::run(int frame)
{
	if (frame == FRAME_NET_UPDATE_START)
		clear();
	else if (frame == FRAME_NET_UPDATE_END)
		fill();
}

void EntityCache::fill()
{
	const int maxIdx = interfaces::entList->getHighestIndex();
	for (auto i : std::views::iota(1, maxIdx))
	{
		auto entity = reinterpret_cast<Entity_t*>(interfaces::entList->getClientEntity(i));
		if (!entity)
			continue;

		// check dormacy yourself

		const auto cl = entity->clientClass();
		if (!cl)
			continue;

		auto classID = cl->m_classID;

		if (classID >= CWeaponAug && classID <= CWeaponXM1014
			|| classID == CAK47 || classID == CDEagle)
		{
			m_entCache[EntCacheType::WEAPON].emplace_back(std::make_tuple(entity, i, classID));
		}

		switch (classID)
		{
		case CCSPlayer:	
			m_entCache[EntCacheType::PLAYER].emplace_back(std::make_tuple(entity, i, classID));
			break;
		case CBaseCSGrenadeProjectile:
		case CDecoyProjectile:
		case CMolotovProjectile:
		case CSmokeGrenadeProjectile:
		case CInferno:
			m_entCache[EntCacheType::GRENADE_PROJECTILES].emplace_back(std::make_tuple(entity, i, classID));
			break;
		case CFogController:
		case CEnvTonemapController:
		case CPostProcessController:
			m_entCache[EntCacheType::CONTROLLERS].emplace_back(std::make_tuple(entity, i, classID));
			break;
		case CPlantedC4:
			m_entCache[EntCacheType::WORLD_ENTS].emplace_back(std::make_tuple(entity, i, classID));
			break;
		default:
			break;
		}
	}
}

void EntityCache::clear()
{
	for (auto& [type, vec] : m_entCache)
		vec.clear();
}
