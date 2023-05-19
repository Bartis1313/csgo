#pragma once

class IGameEvent;

namespace hitmarker
{
	void handleHits(IGameEvent* event);
	void draw();
}