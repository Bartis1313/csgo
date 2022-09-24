#pragma once

#include "math/Vector.hpp"

#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

struct Mstudiobone_t
{
	int m_nameIndex;
	int m_parent;
	PAD(152);
	int m_flags;
	PAD(52);
};

struct Mstudiobbox_t
{
	int m_bone;
	int m_group;
	Vector m_bbmin;
	Vector m_bbmax;
	int m_hitboxnameindex;
	PAD(12);
	float m_radius;
	PAD(16);

	const char* getName() const
	{
		return (!m_hitboxnameindex) ? nullptr : reinterpret_cast<const char*>((uint8_t*)this + m_hitboxnameindex);
	}
};

struct Mstudiohitboxset_t
{
	int m_nameIndex;
	int m_hitboxesCount;
	int m_hitboxesIndex;

	const char* getName() const
	{
		return (!m_nameIndex) ? nullptr : reinterpret_cast<const char*>((uint8_t*)this + m_hitboxesIndex);
	}

	Mstudiobbox_t* getHitbox(int i) const
	{
		return (i > m_hitboxesCount) ? nullptr : reinterpret_cast<Mstudiobbox_t*>((uint8_t*)this + m_hitboxesIndex) + i;
	}
};

struct Model_t
{
	void* m_handle;
	char m_name[260];
	int m_loadFlags;
	int m_serverCount;
	int m_type;
	int m_flags;
	Vector m_vecMins;
	Vector m_vecMaxs;
	float m_radius;
	PAD(28);
};

struct Studiohdr_t
{
	int m_id;
	int m_version;
	long m_checksum;
	char m_name[64];
	int m_length;
	Vector m_eyePos;
	Vector m_illiumPos;
	Vector m_hullMins;
	Vector m_hullMaxs;
	Vector m_mins;
	Vector m_maxs;
	int m_flags;
	int m_bonesCount;
	int m_boneIndex;
	int m_boneControllersCount;
	int m_boneControllerIndex;
	int m_hitboxSetsCount;
	int m_hitboxSetIndex;

	Mstudiohitboxset_t* getHitboxSet(int i) const
	{
		return (i > m_hitboxSetsCount) ? nullptr : reinterpret_cast<Mstudiohitboxset_t*>((uint8_t*)this + m_hitboxSetIndex) + i;
	}

	Mstudiobone_t* getBone(int i) const
	{
		return (i > m_bonesCount) ? nullptr : reinterpret_cast<Mstudiobone_t*>((uint8_t*)this + m_boneIndex) + i;
	}
};

class IVModelInfo
{
public:
	VFUNC(int, getModelIndex, 2, (const char* name), (this, name));
	VFUNC(Studiohdr_t*, getStudioModel, 32, (const Model_t* model), (this, model));
};