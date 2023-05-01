#pragma once

enum FrameStage;
class IGameEvent;

namespace bombOverlay
{
	void run(FrameStage stage);
	void draw();
	void init();
	void roundRestart();
	void handleWhoPlanted(IGameEvent* _event);
}