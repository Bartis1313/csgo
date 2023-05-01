#pragma once

enum FrameStage;

namespace weatherController
{
	// add to imgui
	void implMenu();

	void run(FrameStage stage);
	void reset();
}