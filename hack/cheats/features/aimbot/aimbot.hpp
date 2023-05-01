#pragma once

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

namespace Aimbot
{

	Player_t* getTargetted();
	Vec3 getBestHibox();

	void init();
	void run(float* x, float* y);
	void updateKeys();

	void resetFields();
	[[nodiscard]] bool isClicked(CUserCmd* cmd);
	[[nodiscard]] bool getBestTarget(Weapon_t* wpn, const Vec3& eye, const Vec3& punch);

	[[nodiscard]] std::vector<Hitboxes> getHitboxes();

	inline Player_t* m_bestEnt;
	inline Vec3 m_bestHitpos;
	inline bool m_delay;
	inline float m_delayTime;
	inline Vec3 m_view;
	inline int m_bestId;
	inline IConVar* m_scale = nullptr;
	inline IConVar* m_yaw = nullptr;
	inline IConVar* m_pitch = nullptr;

	inline std::vector<AimbotTarget_t> m_targets;
};