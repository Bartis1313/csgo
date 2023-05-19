#include "levelInitPreEntity.hpp"

hooks::LevelInitPreEntity::value FASTCALL hooks::LevelInitPreEntity::hook(FAST_ARGS, const char* map)
{
	Storage::runs.run();

	original(thisptr, map);
}