#pragma once

class CUserCmd;

namespace plots
{
	void draw();
	void init();

	void drawFps();
	void drawVelocity();

	void collectVelocity(CUserCmd* cmd);
}
