#include "overrideConfig.hpp"

#include <cheats/features/visuals/world/weather/ground.hpp>

hooks::OverrideConfig::value hooks::OverrideConfig::hook(HACK_FAST_ARGS, void* config, bool update)
{
	weather::ground::applyManually();

	auto res = original(thisptr, config, update);

	return res;
}