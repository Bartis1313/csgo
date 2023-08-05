#include "levelInitPreEntity.hpp"

hooks::LevelInitPreEntity::value HACK_FASTCALL hooks::LevelInitPreEntity::hook(HACK_FAST_ARGS, const char* map)
{
	Storage::runs.run();

	original(thisptr, map);
}