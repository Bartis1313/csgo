#include "hooks.hpp"

#include "../classes/frameStage.hpp"

#include <SDK/interfaces/interfaces.hpp>

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	FrameStageType::runAll(frame);

	original(interfaces::client, frame);
}
