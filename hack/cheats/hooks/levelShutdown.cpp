#include "levelShutdown.hpp"

hooks::LevelShudown::value hooks::LevelShudown::hook(HACK_FAST_ARGS)
{
	original(thisptr);
}