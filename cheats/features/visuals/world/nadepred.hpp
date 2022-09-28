#pragma once

#include <classes/createMove.hpp>
#include <classes/onlyInit.hpp>
#include <classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>

#include <vector>

struct Trace_t;
class Entity_t;
class Player_t;
class Nade_t;
enum WeaponIndex;
struct Studiohdr_t;

class GrenadePrediction;

class GreandePredictionButton : public CreateMovePrePredictionType
{
public:
	constexpr GreandePredictionButton() :
		CreateMovePrePredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);

	_NODISCARD int getButton() const { return m_button; }
	_NODISCARD int getWeaponIdx() const { return m_weaponIdx; }
private:
	void runView();
	int m_button;
	int m_weaponIdx;
};

[[maybe_unused]] inline auto g_GrenadePredictionButton = GreandePredictionButton{};

// a lot of code is in cstrike15 leak, as I tried to do this I run on few cases where it was not pixel perfect
// fix is def needed, for breakables bounces
class GrenadePrediction : public RenderableSurfaceType
{
public:
	constexpr GrenadePrediction() :
		RenderableSurfaceType{}
	{}

	friend GreandePredictionButton;

	virtual void init();
	virtual void draw();
private:
	std::vector<Vec3> m_path;
	std::vector<Vec3> m_bounces;
protected:
	void setup(Vec3& src, Vec3& vecThrow, const Vec3& viewangles);
	size_t step(Vec3& src, Vec3& vecThrow, int tick, float interval);
	void simulate();
	bool checkDetonate(const Vec3& vecThrow, const Trace_t& tr, int tick, float interval);
	void addGravityMove(Vec3& move, Vec3& vel, float frametime);
private:
	void traceHull(Vec3& src, Vec3& end, Trace_t& tr);
	void pushEntity(Vec3& src, const Vec3& move, Trace_t& tr);
	void resolveFlyCollisionCustom(Trace_t& tr, Vec3& velocity, const Vec3& move, float interval);
protected:
	void physicsClipVelocity(const Vec3& in, const Vec3& normal, Vec3& out, float overbounce);
};

[[maybe_unused]] inline auto g_GrenadePrediction = GrenadePrediction{};
