#include "hooks.hpp"

#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/classes/frameStage.hpp"

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	FrameStageType::runAll(frame);

	original(interfaces::client, frame);
}
