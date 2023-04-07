#pragma once

#include <cheats/classes/createMove.hpp>
#include <cheats/classes/overrideM.hpp>
#include <cheats/classes/wndProcKeyHandler.hpp>
#include <SDK/math/Vector.hpp>
#include <config/cfgWeapon.hpp>

#include <vector>

class CUserCmd;
class Player_t;
class Weapon_t;
class IConVar;

enum Hitboxes;

struct AimbotTarget_t
{
	Player_t* m_player;
	float m_fov;
	int m_index;
	Hitboxes m_bestHitboxID;
	Vec3 m_pos;
};

class Aimbot : protected OverrideMouseType, protected WndProcKeyHandler
{
public:
	constexpr Aimbot() :
		OverrideMouseType{},
		WndProcKeyHandler{}
	{}

	Player_t* getTargetted() const;
	Vec3 getCachedView() const;
	Vec3 getBestHibox() const;
	CfgWeapon getCachedConfig() const;
protected:
	virtual void init() override;
	virtual void run(float* x, float* y) override;
	virtual void updateKeys() override;
private:
	void resetFields();
	[[nodiscard]] bool isClicked(CUserCmd* cmd);
	[[nodiscard]] bool getBestTarget(Weapon_t* wpn, const Vec3& eye, const Vec3& punch);

	[[nodiscard]] std::vector<Hitboxes> getHitboxes();

	Player_t* m_bestEnt;
	Vec3 m_bestHitpos;
	bool m_delay;
	float m_delayTime;
	Vec3 m_view;
	int m_bestId;
	CfgWeapon m_config;
	IConVar* m_scale = nullptr;
	IConVar* m_yaw = nullptr;
	IConVar* m_pitch = nullptr;

	std::vector<AimbotTarget_t> m_targets;
};

GLOBAL_FEATURE(Aimbot);