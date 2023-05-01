#include "screen2dEffect.hpp"

hooks::Screen2DEffect::value hooks::Screen2DEffect::hook(FAST_ARGS, CViewSetup* view)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });

		return original(thisptr, view);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	Storage::runs.run(view);

	original(thisptr, view);
}