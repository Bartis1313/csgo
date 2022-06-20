#pragma once

class CUserCmd;
struct Vector;

class Movement final
{
public:
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/333797-humanised-bhop.html
	void bunnyhop(CUserCmd* cmd);
	void strafe(CUserCmd* cmd);
	void fix(CUserCmd* cmd, const Vector& oldView);
};

inline Movement movement;