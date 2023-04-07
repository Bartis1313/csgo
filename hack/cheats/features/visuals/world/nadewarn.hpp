#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>

#include <vector>
#include <unordered_map>

struct Trace_t;
class Entity_t;
class Player_t;
class Nade_t;
enum WeaponIndex;
struct Studiohdr_t;
class GrenadeWarningPaint;

// code not fully made by me
// Uses much less math than predicting the full throw due to existance of networkable values
class NadeTrace_t
{
public:
	NadeTrace_t() = default;
	constexpr NadeTrace_t(Player_t* owner, WeaponIndex index) :
		m_nadeOwner{ owner }, m_index{ index }
	{}

	void simulate(const Vec3& pos, const Vec3& velocity, float nadeThrowTime, uint32_t ticks);
	bool draw(WeaponIndex idx);
private:
	void addGravityMove(Vec3& move, Vec3& vel, float frametime);
	void physicsClipVelocity(const Vec3& in, const Vec3& normal, Vec3& out, float overbounce);
	void destroyTrace();
	void push();
	void handleDetonates();
	void handleDestroy();
	bool step(float interval);
	void traceHull(const Vec3& src, const Vec3& end, Entity_t* entity, Trace_t* tr);
	void pushEntity(const Vec3& src, Trace_t& tr);
	void resolveFlyCollisionCustom(Trace_t& tr, float interval);
	std::optional<std::string_view> getConsoleName(WeaponIndex idx) const;

	bool m_detonated = false;
	// nade index
	WeaponIndex m_index = {};
	// current tick for nade loop
	uint32_t m_tick = {};
	// next tick, based on passed addon, to make this more perfect with tick timing
	uint32_t m_nextTick = {};
	// for decoy
	size_t m_bouncesCheck = 0;

	Player_t* m_nadeOwner = nullptr;
	Vec3 m_pos = {};
	Vec3 m_velocity = {};
	float m_nadeDetonateTime = {};
	float m_nadeEndTime = {};
	std::vector<Vec3> m_path = {};
};

class GrenadeWarningPaint : protected RenderableSurfaceType
{
public:
	GrenadeWarningPaint() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw();
private:
	std::unordered_map<int, NadeTrace_t> m_datas = {};
};

GLOBAL_FEATURE(GrenadeWarningPaint);