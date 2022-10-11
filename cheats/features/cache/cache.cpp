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
	const int maxIdx = interfaces::entList->getHighestIndex();
	if (maxIdx <= 1)
		return;

	for (auto i : std::views::iota(1, maxIdx))
	{
		auto entity = reinterpret_cast<Entity_t*>(interfaces::entList->getClientEntity(i));
		if (!entity)
			continue;

		const auto cl = entity->clientClass();
		if (!cl)
			continue;

		size_t index = i;
		size_t classID = cl->m_classID;

		const auto data = HolderData{ entity, index, classID };
		fill(data);
	}
}

void EntityCache::add(Entity_t* ent)
{
	if (!checkRepeatable(ent))
		return;

	const auto possibleIndexes = getIndexes(ent);
	if (!possibleIndexes.has_value())
		return;

	auto [index, classID] = possibleIndexes.value();

	const auto data = HolderData{ ent, index, classID };
	fill(data);
}

std::optional<std::pair<size_t, size_t>> EntityCache::getIndexes(Entity_t* ent)
{
	int i = ent->getIndex();
	if (i <= 1)
		return std::nullopt;

	const auto cl = ent->clientClass();
	if (!cl)
		return std::nullopt;

	size_t index = i;
	size_t classID = cl->m_classID;

	return std::make_pair(index, classID);
}

bool EntityCache::checkRepeatable(Entity_t* ent)
{
	if (!ent)
		return false;

	for (auto [cacheType, ents] : m_entCache)
	{
		if (auto itr = std::ranges::find_if(ents,
			[ent](const HolderData& el)
			{
				return el.ent == ent;
			}
		); itr != ents.cend()) // do not add the same memory place
		{
			return false;
		}
	}

	return true;
}

void EntityCache::fill(const HolderData& data)
{
	auto [ent, index, classID] = data;

	if (classID >= CWeaponAug && classID <= CWeaponXM1014
		|| classID == CAK47 || classID == CDEagle)
	{
		m_entCache[EntCacheType::WEAPON].emplace_back(HolderData{ ent, index, classID });
	}

	switch (classID)
	{
	case CCSPlayer:
		m_entCache[EntCacheType::PLAYER].emplace_back(HolderData{ ent, index, classID });
		break;
	case CBaseCSGrenadeProjectile:
	case CDecoyProjectile:
	case CMolotovProjectile:
	case CSmokeGrenadeProjectile:
	case CInferno:
		m_entCache[EntCacheType::GRENADE_PROJECTILES].emplace_back(HolderData{ ent, index, classID });
		break;
	case CFogController:
	case CEnvTonemapController:
	case CPostProcessController:
		m_entCache[EntCacheType::CONTROLLERS].emplace_back(HolderData{ ent, index, classID });
		break;
	case CPlantedC4:
		m_entCache[EntCacheType::WORLD_ENTS].emplace_back(HolderData{ ent, index, classID });
		break;
	default:
		break;
	}
}

void EntityCache::erase(Entity_t* ent)
{
	for (auto& [cacheType, ents] : m_entCache)
	{
		if (auto itr = std::ranges::find_if(ents,
			[ent](const auto& el)
			{
				return el.ent == ent;
			}
		); itr != ents.cend())
		{
			ents.erase(itr);
		}
	}
}

void EntityCache::clear()
{
	for ([[maybe_unused]] auto& [type, vec] : m_entCache)
		vec.clear();
}
