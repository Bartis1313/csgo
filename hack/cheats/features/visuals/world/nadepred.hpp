#pragma once

class CUserCmd;

// a lot of code is in cstrike15 leak, as I tried to do this I run on few cases where it was not pixel perfect
// fix is def needed, for breakables bounces
namespace grenadePrediction
{
	void draw();
	void run(CUserCmd* cmd);
}
