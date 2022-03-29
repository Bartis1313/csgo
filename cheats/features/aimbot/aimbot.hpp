#pragma once

#include "../../../SDK/math/Vector.hpp"

class CUserCmd;
class Player_t;

class Aimbot final
{
public:
	void drawFov();
	void run(CUserCmd* cmd);
	void runRCS(CUserCmd* cmd);
	void drawBestPoint();
	Player_t* getBest() const;
private:
	void RCS(CUserCmd* cmd);
	void resetBests();
	Player_t* m_bestEnt = nullptr;
	Vector m_bestHitpos = {};
};

inline Aimbot aimbot;