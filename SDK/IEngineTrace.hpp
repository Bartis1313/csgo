#pragma once
#include "math/Vector.hpp"
#include "math/matrix.hpp"
#include "../utilities/pad.hpp"
#include "../utilities/vfunc.hpp"
#include "Enums.hpp"

#pragma warning(disable: 26495)

class Player_t;

class Ray_t
{
public:
	VectorAligned m_start;
	VectorAligned m_delta;
	VectorAligned m_startOffset;
	VectorAligned m_extents;
	const Matrix3x4* m_mattrixWorldAxis;
	bool m_isRay;
	bool m_isSwept;

	Ray_t() = default;
	Ray_t(const Vector& src, const Vector& dest) :
		m_start{ src }, m_delta{ dest - src }, m_mattrixWorldAxis{ nullptr }, m_isRay{ true }
	{
		m_isSwept = m_delta.lengthSqrt();
	}
	Ray_t(const Vector& src, const Vector& dest, const Vector& min, const Vector& max)
	{
		m_delta = (dest - src);

		m_mattrixWorldAxis = nullptr;
		m_isSwept = m_delta.lengthSqrt();

		m_extents = max - min;
		m_extents *= 0.5f;
		m_isRay = m_extents.lengthSqrt() < 1e-6f;

		m_startOffset = min + max;
		m_startOffset *= 0.5f;
		m_start = src + m_startOffset;
		m_startOffset *= -1.0f;
	}
};

struct Csurface_t
{
	const char* m_name;
	short m_props;
	unsigned short m_flags;
};

struct Cplane_t
{
	Vector m_normal;
	float m_distance;
	char m_type;
	char m_signBits;
	PAD(2);
};

struct Trace_t
{
	Vector m_start;
	Vector m_end;
	Cplane_t m_plane;
	float m_fraction;
	int m_contents;
	unsigned short m_flags;
	bool m_allSolid;
	bool m_startSolid;
	float m_fractionLeftSolid;
	Csurface_t m_surface;
	int m_hitgroup;
	short m_physicsBone;
	Player_t* m_entity;
	int m_hitbox;

	bool didHit() const
	{
		return m_fraction < 1.0f || m_allSolid || m_startSolid;
	}
};

class TraceFilter
{
public:
	TraceFilter() = default;
	TraceFilter(void* entity) :
		m_skip{ entity }
	{}
	TraceFilter(void* entity, CollisionGroup_t group) :
		m_skip{ entity }, m_collisionGroup{ group }
	{}
	virtual bool shouldHitEntity(Player_t* ent, int) const
	{
		return ent != m_skip;
	}
	virtual int getTraceType() const
	{
		return m_collisionGroup;
	}
	void* m_skip;
	CollisionGroup_t m_collisionGroup = COLLISION_GROUP_NONE;
};


class IEngineTrace
{
public:
	VFUNC(int, getPointContents, 0, (const Vector& absPosition, int contentsMask), (this, std::cref(absPosition), contentsMask, nullptr));
	VFUNC(void, traceRay, 5, (const Ray_t& ray, unsigned int mask, TraceFilter* filter, Trace_t* trace), (this, std::cref(ray), mask, filter, trace));
};