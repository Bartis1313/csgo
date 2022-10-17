#include "hooks.hpp"

#include "../classes/frameStage.hpp"

void __fastcall hooks::frameStageNotify::hooked(FAST_ARGS, int frame)
{
	FrameStageType::runAll(frame);

	original(thisptr, frame);
}
