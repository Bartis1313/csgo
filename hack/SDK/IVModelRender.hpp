#pragma once

#include "math/Vector.hpp"
#include "math/matrix.hpp"

#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

#include "lightdesc.hpp"

struct model_t;
struct studiohdr_t;
class IMaterial;

struct ModelRenderInfo_t
{
	Vec3 m_origin;
	Vec3 m_angles;
	PAD(4);
	void* m_renderable;
	const model_t* m_model;
	const Matrix3x4* m_modelWorld;
	const Matrix3x4* m_lightOffset;
	const Vec3* m_lightOrigin;
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
	studiohdr_t* m_studioHdr;
	void* m_studioHWData;
	void* m_renderable;
	const Matrix3x4* m_modelToWorld;
	void* m_decals;
	int m_drawFlags;
	int m_lod;
};

struct DrawModelResults_t
{
	/*int m_ActualTriCount;
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	CFastTimer m_RenderTime;
	CUtlVectorFixed<IMaterial*, MAX_DRAW_MODEL_INFO_MATERIALS> m_Materials;*/
};

struct ColorMeshInfo_t
{
	// A given color mesh can own a unique Mesh, or it can use a shared Mesh
	// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
	void* m_mesh;
	void* m_pooledVBAllocator;
	int	m_vertOffsetInBytes;
	int	m_numVerts;
};

enum
{
	MATERIAL_MAX_LIGHT_COUNT = 4,
};

struct MaterialLightingState_t
{
	Vec3 m_vecAmbientCube[6];		// ambient, and lights that aren't in locallight[]
	Vec3 m_vecLightingOrigin;		// The position from which lighting state was computed
	int	m_localLightCount;
	LightDesc_t m_localLightDesc[MATERIAL_MAX_LIGHT_COUNT];
};

struct DrawModelInfo_t
{
	studiohdr_t* m_studioHdr;
	void* m_hardwareData;
	void* m_Decals;
	int	m_Skin;
	int	m_Body;
	int	m_HitboxSet;
	void* m_pClientEntity;
	int	m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool m_bStaticLighting;
	MaterialLightingState_t	m_LightingState;
};

class IVModelRender
{
public:
	VFUNC(void, overrideMaterial, 1, (IMaterial* material, int x = 0, int y = 0), (this, material, x, y));
	VFUNC(bool, isForcedMaterial, 2, (), (this));
};
