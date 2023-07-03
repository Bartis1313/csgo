#include "levelInitPostEnttity.hpp"

#include <gamememory/memory.hpp>

hooks::LevelInitPostEntity::value hooks::LevelInitPostEntity::hook(FAST_ARGS)
{
	memory::postInit();

	Storage::runs.run();

	original(thisptr);
}