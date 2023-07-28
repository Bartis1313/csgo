#pragma once

#include "math/Vector.hpp"

#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

struct mstudiobone_t
{
	int m_nameIndex;
	int m_parent;
	PAD(152);
	int m_flags;
	PAD(52);
};

struct mstudiobbox_t
{
	int m_bone;
	int m_group;
	Vec3 m_bbmin;
	Vec3 m_bbmax;
	int m_hitboxnameindex;
	PAD(12);
	float m_radius;
	PAD(16);

	const char* getName() const
	{
		return (!m_hitboxnameindex) ? nullptr : reinterpret_cast<const char*>((uint8_t*)this + m_hitboxnameindex);
	}
};

struct mstudiohitboxset_t
{
	int m_nameIndex;
	int m_hitboxesCount;
	int m_hitboxesIndex;

	const char* getName() const
	{
		return (!m_nameIndex) ? nullptr : reinterpret_cast<const char*>((uint8_t*)this + m_hitboxesIndex);
	}

	mstudiobbox_t* getHitbox(int i) const
	{
		return (i > m_hitboxesCount) ? nullptr : reinterpret_cast<mstudiobbox_t*>((uint8_t*)this + m_hitboxesIndex) + i;
	}
};

struct model_t
{
	void* m_handle;
	char m_name[260];
	int m_loadFlags;
	int m_serverCount;
	int m_type;
	int m_flags;
	Vec3 m_vecMins;
	Vec3 m_vecMaxs;
	float m_radius;
	PAD(28);
};

struct studiohdr_t
{
	int m_id;
	int m_version;
	long m_checksum;
	char m_name[64];
	int m_length;
	Vec3 m_eyePos;
	Vec3 m_illiumPos;
	Vec3 m_hullMins;
	Vec3 m_hullMaxs;
	Vec3 m_mins;
	Vec3 m_maxs;
	int m_flags;
	int m_bonesCount;
	int m_boneIndex;
	int m_boneControllersCount;
	int m_boneControllerIndex;
	int m_hitboxSetsCount;
	int m_hitboxSetIndex;

	mstudiohitboxset_t* getHitboxSet(int i) const
	{
		return (i > m_hitboxSetsCount) ? nullptr : reinterpret_cast<mstudiohitboxset_t*>((uint8_t*)this + m_hitboxSetIndex) + i;
	}

	mstudiobone_t* getBone(int i) const
	{
		return (i > m_bonesCount) ? nullptr : reinterpret_cast<mstudiobone_t*>((uint8_t*)this + m_boneIndex) + i;
	}
};

class CPhysCollide;

struct vcollide_t
{
	unsigned short solidCount : 15;
	unsigned short isPacked : 1;
	unsigned short descSize;
	// VPhysicsSolids
	CPhysCollide** solids;
	char* pKeyValues;
	void* pUserData;
};

class IVModelInfo
{
public:
	VFUNC(model_t*, getModel, 1, (int modelindex), (this, modelindex));
	VFUNC(int, getModelIndex, 2, (const char* name), (this, name));
	VFUNC(vcollide_t*, getVCollide, 5, (model_t* model), (this, model));
	VFUNC(vcollide_t*, getVCollide, 6, (int modelIdx), (this, modelIdx));
	VFUNC(studiohdr_t*, getStudioModel, 32, (const model_t* model), (this, model));
};