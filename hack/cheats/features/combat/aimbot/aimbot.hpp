#pragma once

class CUserCmd;

namespace Aimbot
{
	void init();
	void runMouse(float* x, float* y);
	void run(CUserCmd* cmd);
	void updateKeys();
};