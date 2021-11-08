#pragma once
#include "IEngineTrace.hpp"
#include "material.hpp"
#include <cstdint>
#include "../utilities/pad.hpp"

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_BELLY,
	HITBOX_THORAX,
	HITBOX_LOWER_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

struct mstudiobone_t
{
	int m_nameIndex;
	int m_parent;
	PAD(168);
	int m_flags;
	PAD(24);
};

struct mstudiobbox_t
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
	Vector m_vecMins;
	Vector m_vecMaxs;
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

	mstudiohitboxset_t* getHitboxSet(int i) const
	{
		return (i > m_hitboxSetsCount) ? nullptr : reinterpret_cast<mstudiohitboxset_t*>((uint8_t*)this + m_hitboxSetIndex) + i;
	}
	mstudiobbox_t* bone(int i) const
	{
		return (i > m_bonesCount) ? nullptr : reinterpret_cast<mstudiobbox_t*>((uint8_t*)this + m_boneIndex) + i;
	}
};

enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,	// has both translucent and opaque sub-partsa
};

class IVModelInfo
{
public:
	VFUNC(int, getModelIndex, 2, (const char* name), (this, name));
	VFUNC(studiohdr_t*, getStudioModel, 32, (const model_t* model), (this, model));
};