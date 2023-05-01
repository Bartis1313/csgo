#include "unknownOverviewMap.hpp"

#include <cheats/features/visuals/radar/radar.hpp>

hooks::UnknownOverviewMap::value hooks::UnknownOverviewMap::hook(FAST_ARGS, int unk)
{
	const int ret = original(thisptr, unk);

	radar::gatherMap(static_cast<MapStruct*>(thisptr));
	
	return ret;
}