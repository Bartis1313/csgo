#pragma once

#include <vector>

#include "../../../SDK/math/Vector.hpp"

class CUserCmd;
class Player_t;
class Weapon_t;

class Aimbot final
{
public:
	void run(CUserCmd* cmd);
	void runRCS(CUserCmd* cmd);
	Player_t* getTargetted();
	void draw();
private:
	void RCS(CUserCmd* cmd);
	void drawFov();
	void drawBestPoint();
	void resetFields();
	bool isClicked(CUserCmd* cmd);
	bool getBestTarget(CUserCmd* cmd, Weapon_t* wpn, const Vector& eye, const Vector& punch);
	std::vector<size_t> getHitboxes();
	Player_t* m_bestEnt = nullptr;
	Vector m_bestHitpos = {};
	bool m_delay = false;
	float m_delayTime = 0.0f;
	Player_t* m_temp;
};

inline Aimbot aimbot;