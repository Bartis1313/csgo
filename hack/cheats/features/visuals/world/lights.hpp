#pragma once

enum FrameStage;

namespace lights
{
	void init();
	void run(FrameStage stage);
	void postReset();
	void forceReset();
	void shutdown();
}