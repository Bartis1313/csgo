#pragma once

class CUserCmd;

class Triggerbot final
{
public:
	void run(CUserCmd* cmd);
};

inline Triggerbot triggerbot;