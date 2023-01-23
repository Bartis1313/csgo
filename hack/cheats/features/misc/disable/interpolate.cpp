#include "interpolate.hpp"

#include <cheats/game/game.hpp>
#include <cheats/features/cache/cache.hpp>
#include <config/vars.hpp>

void Interpolate::run(int frame)
{
	if (!vars::misc->disableItems->interpolate)
		return;

	if (frame != FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		return;

	for (auto [ent, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		const auto entity = reinterpret_cast<Player_t*>(ent);
		const auto map = entity->getVarMap();
		if (!map)
			continue;

		for (auto i : std::views::iota(0, map->m_interpolatedEntries))
		{
			auto entry = &map->m_Entries[i];
			entry->m_needsToInterpolate = false;
		}
	}
}