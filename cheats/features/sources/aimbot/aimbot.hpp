#pragma once

#include "../../classes/createMove.hpp"
#include "../../../../SDK/math/Vector.hpp"

#include <vector>

class CUserCmd;
class Player_t;
class Weapon_t;

class Aimbot : public CreateMoveInPredictionType
{
public:
	Aimbot()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(CUserCmd* cmd);
	Player_t* getTargetted() const;
	Vector getCachedView() const;
	Vector getBestHibox() const;
private:
	void resetFields();
	bool isClicked(CUserCmd* cmd);
	bool getBestTarget(CUserCmd* cmd, Weapon_t* wpn, const Vector& eye, const Vector& punch);

	std::vector<size_t> getHitboxes();
	Player_t* m_bestEnt;
	Vector m_bestHitpos;
	bool m_delay;
	float m_delayTime;
	Vector m_view;
	int m_bestId;
};

[[maybe_unused]] inline auto g_Aimbot = Aimbot{};