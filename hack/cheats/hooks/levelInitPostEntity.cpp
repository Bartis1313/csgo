#include "levelInitPostEnttity.hpp"

#include <gamememory/memory.hpp>

hooks::LevelInitPostEntity::value hooks::LevelInitPostEntity::hook(HACK_FAST_ARGS)
{
	original(thisptr);

	memory::postInit();

	Storage::runs.run();
}