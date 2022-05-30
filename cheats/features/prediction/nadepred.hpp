#pragma once

#include <vector>
#include <unordered_map>

#include "../../../SDK/math/Vector.hpp"

struct Trace_t;
class Entity_t;
class Player_t;
class Nade_t;
enum WeaponIndex;

// a lot of code is in cstrike15 leak, as I tried to do this I run on few cases where it was not pixel perfect
// fix is def needed, for breakables bounces
class GrenadePrediction
{
public:
	void createMove(int buttons);
	void viewSetup();
	void draw();
private:
	std::vector<Vector> m_path;
	std::vector<Vector> m_bounces;
	int m_indexWpn = 0;
	int m_act = 0;
protected:
	void setup(Vector& src, Vector& vecThrow, const Vector& viewangles);
	size_t step(Vector& src, Vector& vecThrow, int tick, float interval);
	void simulate();
	bool checkDetonate(const Vector& vecThrow, const Trace_t& tr, int tick, float interval);
	void addGravityMove(Vector& move, Vector& vel, float frametime);
	uint32_t timeToTicks(float time);
	float ticksToTime(float time);
private:
	void traceHull(Vector& src, Vector& end, Trace_t& tr);
	void pushEntity(Vector& src, const Vector& move, Trace_t& tr);
	void resolveFlyCollisionCustom(Trace_t& tr, Vector& velocity, const Vector& move, float interval);
protected:
	void physicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
};

// code not fully made by me
// Uses much less math than predicting the full throw due to existance of networkable values
class GrenadeWarning : private GrenadePrediction
{
public:
	void run(Nade_t* entity);
private:
	class NadeTrace_t : private GrenadePrediction
	{
	public:
		NadeTrace_t() = default;
		NadeTrace_t(Player_t* owner, WeaponIndex index) :
			m_nadeOwner{ owner }, m_index{ index }
		{}

		void simulate(const Vector& pos, const Vector& velocity, float nadeThrowTime, int ticks);
		bool draw(Entity_t* entity);
	private:
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
	WeaponIndex getIndexByClass(int idx);
};

inline GrenadePrediction nadePred;
inline GrenadeWarning nadeWarning;
