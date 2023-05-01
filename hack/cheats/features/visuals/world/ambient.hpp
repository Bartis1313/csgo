#pragma once

enum FrameStage;

namespace ambientLight
{
	void run(FrameStage stage);
	void init();
	void shutdown();

	inline bool buttonState{ false };
	inline bool pickerState{ false };
}