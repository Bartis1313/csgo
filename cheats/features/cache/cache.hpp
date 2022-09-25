#pragma once

#include <classes/frameStage.hpp>

#include <vector>
#include <unordered_map>
#include <utility>

class Entity_t;

enum class EntCacheType // add more if needed, each group should contain many idx's, player is the exception
{
	PLAYER,
	GRENADE_PROJECTILES,
	CONTROLLERS, // fog, tone, ambient...
	WORLD_ENTS, // c4 and such...
	WEAPON,
};

// might be also run with: present & reset
class EntityCache : public FrameStageType
{
public:
	EntityCache() : 
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
	// ents
	auto getCache(const EntCacheType& type) { return m_entCache[type]; }
	// map
	auto getCache() const { return m_entCache; }
private:
	void fill(); // FRAME_NET_UPDATE_END
	void clear(); // FRAME_NET_UPDATE_START
	// pointer to ent, index, classID
	using typeCont = std::tuple<Entity_t*, size_t, size_t>;
	std::unordered_map<EntCacheType, std::vector<typeCont>> m_entCache;
};

[[maybe_unused]] inline auto g_EntCache = EntityCache{};