#include "interpolate.hpp"

#include <cheats/game/game.hpp>
#include <cheats/features/cache/cache.hpp>
#include <config/vars.hpp>

#include <cheats/hooks/frameStageNotify.hpp>

namespace
{
	struct InterpolateHandler : hooks::FrameStageNotify
	{
		InterpolateHandler()
		{
			this->registerRun(interpolate::run);
		}
	} interpolateHandler;
}

void interpolate::run(FrameStage stage)
{
	if (!vars::misc->disableItems->interpolate)
		return;

	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		return;

	for (const auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		const auto player = entity->cast<Player_t*>();
		const auto map = player->getVarMap();
		if (!map)
			continue;

		for (int i = 0; i < map->m_interpolatedEntries; ++i)
		{
			const auto entry = &map->m_Entries[i];
			entry->m_needsToInterpolate = false;
		}
	}
}