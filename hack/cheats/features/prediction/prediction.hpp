#pragma once

class CUserCmd;

namespace prediction
{
	void init();
	// move to allocation hook
	void patchDatamap();

	void begin(CUserCmd* cmd);
	void end();
}
