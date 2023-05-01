#include "overrideMouse.hpp"

hooks::OverrideMouse::value hooks::OverrideMouse::hook(FAST_ARGS, float* x, float* y)
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::intis.run(); });

	original(thisptr, x, y);

	Storage::runs.run(x, y);
}