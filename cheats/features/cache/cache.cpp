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

		size_t index = i;
		size_t classID = cl->m_classID;

		const auto data = HolderData{ .ent = entity, .idx = index, .classID = classID };
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

	const auto data = HolderData{ .ent = ent, .idx = index, .classID = classID };
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
	if (auto id = data.classID; id >= CWeaponAug && id <= CWeaponXM1014
		|| id == CAK47 || id == CDEagle)
	{
		m_entCache[EntCacheType::WEAPON].push_back(data);
	}

	switch (data.classID)
	{
	case CCSPlayer:
		m_entCache[EntCacheType::PLAYER].push_back(data);
		break;
	case CBaseCSGrenadeProjectile:
	case CDecoyProjectile:
	case CMolotovProjectile:
	case CSmokeGrenadeProjectile:
	case CInferno:
		m_entCache[EntCacheType::GRENADE_PROJECTILES].push_back(data);
		break;
	case CFogController:
	case CEnvTonemapController:
	case CPostProcessController:
		m_entCache[EntCacheType::CONTROLLERS].push_back(data);
		break;
	case CPlantedC4:
		m_entCache[EntCacheType::WORLD_ENTS].push_back(data);
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