#include "cache.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <game/game.hpp>

#include <ranges>

const auto predictor = [](const EntityCache::HolderData& lhs, const EntityCache::HolderData& rhs)
{
	// do comparison by length to our location
	const auto lhsToLocal = lhs.ent->absOrigin() - game::localPlayer->absOrigin();
	const auto rhsToLocal = rhs.ent->absOrigin() - game::localPlayer->absOrigin();
	return lhsToLocal.length() > rhsToLocal.length();
};

void EntityCache::init()
{
	const int maxIdx = memory::interfaces::entList->getHighestIndex();
	if (maxIdx <= 1)
		return;

	HolderData data{};
	for (auto i : std::views::iota(1, maxIdx))
	{
		auto entity = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientEntity(i));
		if (!entity)
			continue;

		const auto cl = entity->clientClass();
		if (!cl)
			continue;

		size_t index = i;
		size_t classID = cl->m_classID;

		data = HolderData{ .ent = entity, .idx = index, .classID = classID };
		fill(data);
	}

	// not really useful on add since positions are nulled exactly at this frame, use sort in loop
	if (data.classID == CCSPlayer)
		std::ranges::sort(m_entCache.at(EntCacheType::PLAYER), predictor);
}

void EntityCache::add(Entity_t* ent)
{
	if (!checkRepeatable(ent))
		return;

	const auto possibleIndexes = getIndexes(ent);
	if (!possibleIndexes.has_value())
		return;

	auto [index, classID] = possibleIndexes.value();

	const auto data = HolderData{ .ent = ent, .idx = index, .classID = classID };
	fill(data);

	// not really useful on add since positions are nulled exactly at this frame, use sort in loop
	if (data.classID == CCSPlayer)
	{
		// don't use any sort on already sorted list
		auto it = std::ranges::upper_bound(m_entCache.at(EntCacheType::PLAYER), data, predictor);
		if (it != m_entCache.at(EntCacheType::PLAYER).end())
			m_entCache.at(EntCacheType::PLAYER).insert(it, data);
	}
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
	if (auto id = data.classID; id >= CWeaponAug && id <= CWeaponXM1014
		|| id == CAK47 || id == CDEagle)
	{
		m_entCache[EntCacheType::WEAPON].emplace_back(data);
	}

	switch (data.classID)
	{
	case CCSPlayer:
		m_entCache[EntCacheType::PLAYER].emplace_back(data);
		break;
	case CBaseCSGrenadeProjectile:
	case CDecoyProjectile:
	case CMolotovProjectile:
	case CSmokeGrenadeProjectile:
	case CInferno:
		m_entCache[EntCacheType::GRENADE_PROJECTILES].emplace_back(data);
		break;
	case CFogController:
	case CEnvTonemapController:
	case CPostProcessController:
		m_entCache[EntCacheType::CONTROLLERS].emplace_back(data);
		break;
	case CPlantedC4:
		m_entCache[EntCacheType::WORLD_ENTS].emplace_back(data);
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

#include <gamememory/memory.hpp>

void CacheFields::update()
{
	const auto hud = Memory::Address<CCSGO_HudRadar*>{ game::findHudElement("CCSGO_HudRadar") };
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
			m_players.at(player.m_index) = player;
	}
}