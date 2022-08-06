#pragma once

#include "../../../classes/onlyInit.hpp"

#include <vector>
#include <unordered_map>

#include "../../../SDK/math/Vector.hpp"

struct Trace_t;
class Entity_t;
class Player_t;
class Nade_t;
enum WeaponIndex;
struct Studiohdr_t;

// code not fully made by me
// Uses much less math than predicting the full throw due to existance of networkable values
class GrenadeWarning : public OnlyInitType
{
public:
	GrenadeWarning()
	{
		m_hacks.push_back(this);
	}

	virtual void init();
	void run(Nade_t* entity);
private:
	inline static uintptr_t m_traceFilterSimpleAddr;

	class NadeTrace_t
	{
	public:
		NadeTrace_t() = default;
		NadeTrace_t(Player_t* owner, WeaponIndex index) :
			m_nadeOwner{ owner }, m_index{ index }
		{}

		void simulate(const Vector& pos, const Vector& velocity, float nadeThrowTime, int ticks);
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
public:
	// studio is special case arg - because henade and flashbang have exactly same class id
	// and people who think definition index is ok are wrong, player ents DON'T hold the nade anymore in that case
	WeaponIndex getIndexByClass(int idx, Studiohdr_t* studio);
};

[[maybe_unused]] inline auto g_GrenadeWarning = GrenadeWarning{};
