#pragma once

class CUserCmd;

class Bunnyhop final
{
public:
	void run(CUserCmd* cmd);
	void strafe(CUserCmd* cmd);
};

inline Bunnyhop bunnyhop;