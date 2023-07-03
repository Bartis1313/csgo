#include "cache.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <cheats/game/game.hpp>

#include <ranges>

//const auto predictor = [](const EntityCache::HolderData& lhs, const EntityCache::HolderData& rhs)
//{
//	// do comparison by length to our location
//	const auto lhsToLocal = lhs.ent->absOrigin() - game::localPlayer->absOrigin();
//	const auto rhsToLocal = rhs.ent->absOrigin() - game::localPlayer->absOrigin();
//	return lhsToLocal.length() > rhsToLocal.length();
//};

void EntityCache::init()
{
	const int maxIdx = memory::interfaces::entList->getHighestIndex();
	if (maxIdx <= 1)
		return;

	for (auto i : std::views::iota(1, maxIdx))
	{
		auto entity = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientEntity(i));
		if (!entity)
			continue;

		const auto cl = entity->clientClass();
		if (!cl)
			continue;

		HolderData data{ .ent = entity, .idx = i, .classID = cl->m_classID };
		fill(data);
	}
}

#define CACHE_LOGGING false

void EntityCache::add(Entity_t* ent)
{
#ifdef _DEBUG
#if CACHE_LOGGING == true
	auto cl = ent->clientClass();
	if (!cl)
		printf("couldn't get the entity clientclass\n");
	else
		printf("adding entity: %s enum: %i\n", cl->m_networkName, cl->m_classID);
#endif
#endif

	if (!checkRepeatable(ent))
		return;

	const auto possibleIndexes = getIndexes(ent);
	if (!possibleIndexes.has_value())
		return;

	const auto [index, classID] = possibleIndexes.value();

	const auto data = HolderData{ .ent = ent, .idx = index, .classID = classID };
	fill(data);
}

std::optional<std::pair<int, ClassID>> EntityCache::getIndexes(Entity_t* ent)
{
	int i = ent->getIndex();
	if (i <= 1)
		return std::nullopt;

	const auto cl = ent->clientClass();
	if (!cl)
		return std::nullopt;

	return std::make_pair(i, cl->m_classID);
}

bool EntityCache::checkRepeatable(Entity_t* ent)
{
	if (!ent)
		return false;

	for (const auto& [cacheType, ents] : entCacheMap)
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
	constexpr std::array specialWeaponIds = 
	{
		CAK47, CDEagle, CHEGrenade, CDecoyGrenade, CIncendiaryGrenade, CMolotovGrenade, CSensorGrenade, CSmokeGrenade, CFlashbang
	};

	if (auto id = data.classID; id >= CWeaponAug && id <= CWeaponXM1014
		|| std::ranges::find(specialWeaponIds, id) != specialWeaponIds.cend())
	{
		entCacheMap[EntCacheType::WEAPON].push_back(data);
	}

	switch (data.classID)
	{
	case CCSPlayer:
		entCacheMap[EntCacheType::PLAYER].push_back(data);
		break;
	case CBaseCSGrenadeProjectile:
	case CDecoyProjectile:
	case CMolotovProjectile:
	case CSmokeGrenadeProjectile:
	case CInferno:
		entCacheMap[EntCacheType::GRENADE_PROJECTILES].push_back(data);
		break;
	case CPlantedC4:
		entCacheMap[EntCacheType::WORLD_ENTS].push_back(data);
		break;
	default:
		break;
	}
}

void EntityCache::erase(Entity_t* ent)
{
	for (auto& [cacheType, ents] : entCacheMap)
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
	for ([[maybe_unused]] auto& [type, vec] : entCacheMap)
		vec.clear();
}

#include <gamememory/memory.hpp>

void EntityCache::CacheFields::update()
{
	const auto hud = memory::Address<CCSGO_HudRadar*>{ game::findHudElement<CCSGO_HudRadar*>("CCSGO_HudRadar") };
	if (!hud())
		return;

	const auto radar = hud.sub(0x14).cast<CSRadar*>()();

	for (int i = 1; i <= hud->m_sizeOfPlayers + 1; i++)
	{
		const auto& player = radar->m_players[i];

		//player.write();

		if (const auto idx = player.m_index; !idx)
			continue;
		else
			cachedPlayers.at(player.m_index) = player;
	}
}