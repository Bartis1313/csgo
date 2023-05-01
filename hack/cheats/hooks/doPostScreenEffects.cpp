#include "doPostScreenEffects.hpp"

hooks::DoPostScreenEffects::value hooks::DoPostScreenEffects::hook(FAST_ARGS, int val)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(thisptr, val);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	Storage::runs.run();

	return original(thisptr, val);
}