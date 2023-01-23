#include "hooks.hpp"

#include "../classes/frameStage.hpp"

hooks::frameStageNotify::value FASTCALL hooks::frameStageNotify::hooked(FAST_ARGS, int frame)
{
	FrameStageType::runAll(frame);

	original(thisptr, frame);
}
