#pragma once
#include "material.hpp"
#include "math/VMatrix.hpp"
#include "../utilities/utilities.hpp"


struct ModelRenderInfo_t
{
	Vector m_origin;
	Vector m_angles;
	PAD(4);
	void* m_renderable;
	const model_t* m_model;
	const matrix3x4_t* m_modelWorld;
	const matrix3x4_t* m_lightOffset;
	const Vector* m_lightOrigin;
	int m_flags;
	int m_entIndex;
	int m_skin;
	int m_body;
	int m_hitboxset;
	unsigned short m_instance;
	ModelRenderInfo_t()
	{
		m_modelWorld = NULL;
		m_lightOffset = NULL;
		m_lightOrigin = NULL;
	}
};

struct DrawModelState_t
{
	studiohdr_t* m_pStudioHdr;
	void* m_pStudioHWData;
	void* m_pRenderable;
	const matrix3x4_t* m_pModelToWorld;
	void* m_decals;
	int m_drawFlags;
	int m_lod;
};


class IVModelRender
{
public:
	VFUNC(void, overrideMaterial, 1, (IMaterial* material, int x = 0, int y = 0), (this, material, x, y));
	VFUNC(bool, isForcedMaterial, 2, (), (this));
};
