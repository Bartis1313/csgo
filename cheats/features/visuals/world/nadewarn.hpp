#pragma once

#include <classes/onlyInit.hpp>
#include <classes/renderableToSurface.hpp>
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
class GrenadeWarning : public OnlyInitType
{
public:
	GrenadeWarning() :
		OnlyInitType{}
	{}

	virtual void init();
private:
	class NadeTrace_t
	{
	public:
		NadeTrace_t() = default;
		constexpr NadeTrace_t(Player_t* owner, WeaponIndex index) :
			m_nadeOwner{ owner }, m_index{ index }
		{}

		void simulate(const Vector& pos, const Vector& velocity, float nadeThrowTime, uint32_t ticks);
		bool draw(Entity_t* entity, WeaponIndex idx);
	private:
		void addGravityMove(Vector& move, Vector& vel, float frametime);
		void physicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
		void destroyTrace();
		void push();
		void handleDetonates();
		void handleDestroy();
		bool step(float interval);
		void traceHull(const Vector& src, const Vector& end, Entity_t* entity, Trace_t* tr);
		void pushEntity(const Vector& src, Trace_t& tr);
		void resolveFlyCollisionCustom(Trace_t& tr, float interval);

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
		Vector m_pos = {};
		Vector m_velocity = {};
		float m_nadeDetonateTime = {};
		float m_nadeEndTime = {};
		std::vector<Vector> m_path = {};
	};

	std::unordered_map<int, NadeTrace_t> m_datas = {};
	friend GrenadeWarningPaint;
};

[[maybe_unused]] inline auto g_GrenadeWarning = GrenadeWarning{};

class GrenadeWarningPaint : public RenderableSurfaceType
{
public:
	constexpr GrenadeWarningPaint() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_GrenadeWarningPaint = GrenadeWarningPaint{};