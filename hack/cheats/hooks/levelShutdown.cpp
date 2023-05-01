#include "levelShutdown.hpp"

hooks::LevelShudown::value hooks::LevelShudown::hook(FAST_ARGS)
{
	original(thisptr);
}