#include "frameStageNotify.hpp"

hooks::FrameStageNotify::value hooks::FrameStageNotify::hook(FAST_ARGS, FrameStage stage)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(thisptr, stage);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	Storage::runs.run(stage);

	original(thisptr, stage);
}
