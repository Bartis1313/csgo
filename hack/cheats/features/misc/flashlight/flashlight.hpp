#pragma once

enum FrameStage;

namespace flashlight
{
	void run(FrameStage stage);
	void shutdown();
	void updateKeys();
}