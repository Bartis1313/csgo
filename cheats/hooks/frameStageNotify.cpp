#include "hooks.hpp"

#include "../features/classes/frameStage.hpp"

#include <SDK/interfaces/interfaces.hpp>

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	FrameStageType::runAll(frame);

	original(interfaces::client, frame);
}
