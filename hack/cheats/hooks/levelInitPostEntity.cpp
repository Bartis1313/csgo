#include "levelInitPostEnttity.hpp"

#include <gamememory/memory.hpp>

hooks::LevelInitPostEntity::value hooks::LevelInitPostEntity::hook(FAST_ARGS)
{
	memory::postInit();

	original(thisptr);
}