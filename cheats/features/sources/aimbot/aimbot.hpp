#pragma once

#include "../../classes/createMove.hpp"
#include "../../../../SDK/math/Vector.hpp"
#include "../../../../config/cfgWeapon.hpp"

#include <vector>

class CUserCmd;
class Player_t;
class Weapon_t;

class Aimbot : public CreateMoveInPredictionType
{
public:
	constexpr Aimbot() :
		CreateMoveInPredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);
	Player_t* getTargetted() const;
	Vector getCachedView() const;
	Vector getBestHibox() const;
	CfgWeapon getCachedConfig() const;
private:
	void resetFields();
	[[nodiscard]] bool isClicked(CUserCmd* cmd);
	[[nodiscard]] bool getBestTarget(CUserCmd* cmd, Weapon_t* wpn, const Vector& eye, const Vector& punch);

	[[nodiscard]]  std::vector<size_t> getHitboxes();
	Player_t* m_bestEnt;
	Vector m_bestHitpos;
	bool m_delay;
	float m_delayTime;
	Vector m_view;
	int m_bestId;
	CfgWeapon m_config;
};

[[maybe_unused]] inline auto g_Aimbot = Aimbot{};
