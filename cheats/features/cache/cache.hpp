#pragma once

#include <classes/frameStage.hpp>

#include <vector>
#include <unordered_map>
#include <utility>
#include <optional>

class Entity_t;

enum class EntCacheType // add more if needed, each group should contain many idx's, player is the exception
{
	PLAYER,
	GRENADE_PROJECTILES,
	CONTROLLERS, // fog, tone, ambient...
	WORLD_ENTS, // c4 and such...
	WEAPON,
};

class EntityCache
{
public:
	static void add(Entity_t* ent);
	static void erase(Entity_t* ent);
	static void clear();
	// we need to call this once
	// manual init is a thing due to unload during the round
	static void init();
private:
	struct HolderData
	{
		Entity_t* ent;
		size_t idx;
		size_t classID;
	};
	static void fill(const HolderData& data);
	static bool checkRepeatable(Entity_t* ent);
	static std::optional<std::pair<size_t, size_t>> getIndexes(Entity_t* ent);
public:
	inline static auto getCache(const EntCacheType& type)
	{
		return m_entCache[type];
	}
private:
	inline static std::unordered_map<EntCacheType, std::vector<HolderData>> m_entCache;
};