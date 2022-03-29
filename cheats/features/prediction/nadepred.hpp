#pragma once

#include <vector>

struct Trace_t;
struct Vector;

// a lot of code is in cstrike15 leak, as I tried to do this I run on few cases where it was not pixel perfect
// fix is def needed, for breakables bounces

// TODO: add proper physics, this is now just working, not perfectly

class GrenadePrediction
{
public:
	virtual void createMove(int buttons);
	virtual void viewSetup();
	virtual void draw();
protected: // protected because you may with to make something after nade is thrown, drawn path AFTER throw as example...
	std::vector<Vector> m_path;
	std::vector<Vector> m_bounces;
	int m_indexWpn = 0;
	int m_act = 0;

	virtual void setup(Vector& src, Vector& vecThrow, const Vector& viewangles);
	virtual size_t step(Vector& src, Vector& vecThrow, int tick, float interval);
	virtual void simulate();
	virtual bool checkDetonate(const Vector& vecThrow, const Trace_t& tr, int tick, float interval);
	virtual void traceHull(Vector& src, Vector& end, Trace_t& tr);
	virtual void addGravityMove(Vector& move, Vector& vel, float frametime);
	virtual void pushEntity(Vector& src, const Vector& move, Trace_t& tr);
	virtual void resolveFlyCollisionCustom(Trace_t& tr, Vector& velocity, const Vector& move, float interval);
	virtual void physicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
};

inline GrenadePrediction nadePred;