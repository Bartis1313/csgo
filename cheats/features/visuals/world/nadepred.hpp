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
	std::vector<Vector> m_path;
	std::vector<Vector> m_bounces;
protected:
	void setup(Vector& src, Vector& vecThrow, const Vector& viewangles);
	size_t step(Vector& src, Vector& vecThrow, int tick, float interval);
	void simulate();
	bool checkDetonate(const Vector& vecThrow, const Trace_t& tr, int tick, float interval);
	void addGravityMove(Vector& move, Vector& vel, float frametime);
private:
	void traceHull(Vector& src, Vector& end, Trace_t& tr);
	void pushEntity(Vector& src, const Vector& move, Trace_t& tr);
	void resolveFlyCollisionCustom(Trace_t& tr, Vector& velocity, const Vector& move, float interval);
protected:
	void physicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
};

[[maybe_unused]] inline auto g_GrenadePrediction = GrenadePrediction{};
