#pragma once

#include "math/Vector.hpp"
#include "math/matrix.hpp"

#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

struct Model_t;
struct Studiohdr_t;
class IMaterial;

struct ModelRenderInfo_t
{
	Vector m_origin;
	Vector m_angles;
	PAD(4);
	void* m_renderable;
	const Model_t* m_model;
	const Matrix3x4* m_modelWorld;
	const Matrix3x4* m_lightOffset;
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
	Studiohdr_t* m_studioHdr;
	void* m_studioHWData;
	void* m_renderable;
	const Matrix3x4* m_modelToWorld;
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
