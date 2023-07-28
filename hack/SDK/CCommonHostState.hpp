#pragma once

#include "IVModelRender.hpp"
#include "helpers/pad.hpp"
#include <SDK/CUtlVector.hpp>
#include <render/Color.hpp>

#include <gamememory/memory.hpp>

struct decal_t
{
	decal_t* pnext;				// linked list for each surface
	decal_t* pDestroyList;
	void* surfID;		// Surface id for persistence / unlinking
	IMaterial* material;
	float				lightmapOffset;

	// FIXME:
	// make dx and dy in decal space and get rid of position, so that
	// position can be rederived from the decal basis.
	Vec3		position;		// location of the decal center in world space.
	Vec3		saxis;			// direction of the s axis in world space
	float		dx;				// Offsets into surface texture (in texture coordinates, so we don't need floats)
	float		dy;
	float		scale;			// Pixel scale
	float		flSize;			// size of decal, used for rejecting on dispinfo planes
	float		fadeDuration;				// Negative value means to fade in
	float		fadeStartTime;
	SDKColor		color;
	void* userdata;		// For player decals only, decal index ( first player at slot 1 )
	uint16_t	m_DispDecal;	// Handle to displacement decals associated with this
	unsigned short		clippedVertCount;
	unsigned short		cacheHandle;
	unsigned short		m_iDecalPool;		// index into the decal pool.
	short		flags;			// Decal flags  DECAL_*		!!!SAVED AS A BYTE (SEE HOST_CMD.C)
	short		entityIndex;	// Entity this is attached to
	int			m_nTickCreated; // tick on which the decal was created

	// NOTE: The following variables are dynamic variables.
	// We could put these into a separate array and reference them
	// by index to reduce memory costs of this...

	int			m_iSortTree;			// MaterialSort tree id
	int			m_iSortMaterial;		// MaterialSort id.
};
static_assert(sizeof(decal_t) == 100U);

struct msurface2_t;
typedef msurface2_t* SurfaceHandle_t;
struct worldbrushdata_t;

struct msurface2_t
{
	unsigned int			flags;			// see SURFDRAW_ #defines (only 22-bits right now)
	// These are packed in to flags now
	//unsigned char			vertCount;		// number of verts for this surface
	//unsigned char			sortGroup;		// only uses 2 bits, subdivide?
#ifdef _PS3
	cplane_t				m_plane;			// pointer to shared plane
#else
	void* plane;			// pointer to shared plane
#endif
	int						firstvertindex;	// look up in model->vertindices[] (only uses 17-18 bits?)
	unsigned short		decals;
	unsigned short		m_ShadowDecals; // unsigned short
	unsigned short m_nFirstOverlayFragment;	// First overlay fragment on the surface (short)
	short					materialSortID;
	unsigned short			vertBufferIndex;

	unsigned short			m_bDynamicShadowsEnabled : 1;	// Can this surface receive dynamic shadows?
	unsigned short			texinfo : 15;

	void* pDispInfo;         // displacement map information
	int						visframe;		// should be drawn when node is crossed
};

// This structure contains the information used to create new decals
struct decalinfo_t
{
	Vec3		m_Position;			// world coordinates of the decal center
	Vec3		m_SAxis;			// the s axis for the decal in world coordinates
	model_t *m_pModel;			// the model the decal is going to be applied in
	worldbrushdata_t* m_pBrush;		// The shared brush data for this model
	IMaterial* m_pMaterial;		// The decal material
	float		m_Size;				// Size of the decal (in world coords)
	int			m_Flags;
	int			m_Entity;			// Entity the decal is applied to.
	float		m_scale;
	float		m_flFadeDuration;
	float		m_flFadeStartTime;
	int			m_decalWidth;
	int			m_decalHeight;
	SDKColor		m_Color;
	Vec3		m_Basis[3];
	void* m_pUserData;
	const Vec3* m_pNormal;
	CUtlVector<SurfaceHandle_t>	m_aApplySurfs;
};

enum emittype_t
{
	emit_surface,		// 90 degree spotlight
	emit_point,			// simple point light source
	emit_spotlight,		// spotlight with penumbra
	emit_skylight,		// directional light with no falloff (surface must trace to SKY texture)
	emit_quakelight,	// linear falloff, non-lambertian
	emit_skyambient,	// spherical light source with no falloff (surface must trace to SKY texture)
};

struct dworldlight_t
{
	//DECLARE_BYTESWAP_DATADESC();
	Vec3		origin;
	Vec3		intensity;
	Vec3		normal;			// for surfaces and spotlights
	Vec3		shadow_cast_offset;
	int			cluster;
	emittype_t	type;
	int			style;
	float		stopdot;		// start of penumbra for emit_spotlight
	float		stopdot2;		// end of penumbra for emit_spotlight
	float		exponent;		// 
	float		radius;			// cutoff distance
	// falloff for emit_spotlight + emit_point: 
	// 1 / (constant_attn + linear_attn * dist + quadratic_attn * dist^2)
	float		constant_attn;
	float		linear_attn;
	float		quadratic_attn;
	int			flags;			// Uses a combination of the DWL_FLAGS_ defines.
	int			texinfo;		// 
	int			owner;			// entity that this light it relative to
};
static_assert(sizeof(dworldlight_t) == 100U);

struct mtexinfo_t
{
	Vec4	textureVecsTexelsPerWorldUnits[2];	// [s/t] unit vectors in world space. 
	// [i][3] is the s/t offset relative to the origin.
	Vec4	lightmapVecsLuxelsPerWorldUnits[2];
	float		luxelsPerWorldUnit;
	float		worldUnitsPerLuxel;
	unsigned short flags;		// SURF_ flags.
	unsigned short texinfoFlags;// TEXINFO_ flags.
	IMaterial* material;

	mtexinfo_t(mtexinfo_t const& src)
	{
		// copy constructor needed since Vector4D has no copy constructor
		memcpy(this, &src, sizeof(mtexinfo_t));
	}
};


struct worldbrushdata_t
{
	PAD(92);
	int numtexinfo;
	mtexinfo_t* textinfo;
	PAD(16);
	int numsurfaces;
	struct msurface1_t* surfaces1;
	msurface2_t* surfaces2;
	PAD(40);
	int			numworldlights;
	dworldlight_t* worldlights;
	////...
};

class CCommonHostState
{
public:
	// worlsmodel doesn't exist here, due to CModelLoader
	//// cl_entitites[0].model
	//Model_t* worldmodel;
	worldbrushdata_t worldbrush;
	// Tick interval for game
	float interval_per_tick;
};

inline SurfaceHandle_t SurfaceHandleFromIndex(int surfaceIndex, worldbrushdata_t* pData)
{
	return &pData->surfaces2[surfaceIndex];
}

inline unsigned short& MSurf_Decals(SurfaceHandle_t surfID)
{
	return surfID->decals;
}

static inline decal_t* MSurf_DecalPointer(SurfaceHandle_t surfID)
{
	unsigned short handle = MSurf_Decals(surfID);
	constexpr int WORLD_DECAL_HANDLE_INVALID = 0xFFFF;
	if (handle == WORLD_DECAL_HANDLE_INVALID)
		return NULL;

	return memory::decalPool()[handle];
}
