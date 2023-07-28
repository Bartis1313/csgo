#pragma once

#include <render/Color.hpp>
#include "math/Vector.hpp"
#include "KeyValues.hpp"

#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

enum OverrideType_t
{
    OVERRIDE_NORMAL = 0,
    OVERRIDE_BUILD_SHADOWS,
    OVERRIDE_DEPTH_WRITE,
    OVERRIDE_SELECTIVE,
    OVERRIDE_SSAO_DEPTH_WRITE,
};

enum MaterialVarFlags_t
{
    MATERIAL_VAR_DEBUG = (1 << 0),
    MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
    MATERIAL_VAR_NO_DRAW = (1 << 2),
    MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
    MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
    MATERIAL_VAR_VERTEXALPHA = (1 << 5),
    MATERIAL_VAR_SELFILLUM = (1 << 6),
    MATERIAL_VAR_ADDITIVE = (1 << 7),
    MATERIAL_VAR_ALPHATEST = (1 << 8),
    //MATERIAL_VAR_UNUSED = (1 << 9),
    MATERIAL_VAR_ZNEARER = (1 << 10),
    MATERIAL_VAR_MODEL = (1 << 11),
    MATERIAL_VAR_FLAT = (1 << 12),
    MATERIAL_VAR_NOCULL = (1 << 13),
    MATERIAL_VAR_NOFOG = (1 << 14),
    MATERIAL_VAR_IGNOREZ = (1 << 15),
    MATERIAL_VAR_DECAL = (1 << 16),
    MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
    MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
    MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
    MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
    MATERIAL_VAR_TRANSLUCENT = (1 << 21),
    MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
    MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
    MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
    MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
    MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
    MATERIAL_VAR_HALFLAMBERT = (1 << 27),
    MATERIAL_VAR_WIREFRAME = (1 << 28),
    MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
    MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
    MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

//-----------------------------------------------------------------------------
// Internal flags not accessible from outside the material system. Stored in Flags2
//-----------------------------------------------------------------------------
enum MaterialVarFlags2_t
{
    // NOTE: These are for $flags2!!!!!
    //	UNUSED											= (1 << 0),

    MATERIAL_VAR2_LIGHTING_UNLIT = 0,
    MATERIAL_VAR2_LIGHTING_VERTEX_LIT = (1 << 1),
    MATERIAL_VAR2_LIGHTING_LIGHTMAP = (1 << 2),
    MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP = (1 << 3),
    MATERIAL_VAR2_LIGHTING_MASK =
    (MATERIAL_VAR2_LIGHTING_VERTEX_LIT |
        MATERIAL_VAR2_LIGHTING_LIGHTMAP |
        MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP),

    // FIXME: Should this be a part of the above lighting enums?
    MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL = (1 << 4),
    MATERIAL_VAR2_USES_ENV_CUBEMAP = (1 << 5),
    MATERIAL_VAR2_NEEDS_TANGENT_SPACES = (1 << 6),
    MATERIAL_VAR2_NEEDS_SOFTWARE_LIGHTING = (1 << 7),
    // GR - HDR path puts lightmap alpha in separate texture...
    MATERIAL_VAR2_BLEND_WITH_LIGHTMAP_ALPHA = (1 << 8),
    MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS = (1 << 9),
    MATERIAL_VAR2_USE_FLASHLIGHT = (1 << 10),
    MATERIAL_VAR2_USE_FIXED_FUNCTION_BAKED_LIGHTING = (1 << 11),
    MATERIAL_VAR2_NEEDS_FIXED_FUNCTION_FLASHLIGHT = (1 << 12),
    MATERIAL_VAR2_USE_EDITOR = (1 << 13),
    MATERIAL_VAR2_NEEDS_POWER_OF_TWO_FRAME_BUFFER_TEXTURE = (1 << 14),
    MATERIAL_VAR2_NEEDS_FULL_FRAME_BUFFER_TEXTURE = (1 << 15),
    MATERIAL_VAR2_IS_SPRITECARD = (1 << 16),
    MATERIAL_VAR2_USES_VERTEXID = (1 << 17),
    MATERIAL_VAR2_SUPPORTS_HW_SKINNING = (1 << 18),
    MATERIAL_VAR2_SUPPORTS_FLASHLIGHT = (1 << 19),
    MATERIAL_VAR2_USE_GBUFFER0 = (1 << 20),
    MATERIAL_VAR2_USE_GBUFFER1 = (1 << 21),
    MATERIAL_VAR2_SELFILLUMMASK = (1 << 22),
    MATERIAL_VAR2_SUPPORTS_TESSELLATION = (1 << 23)
};

class IMaterialVar;

enum ShaderMaterialVars_t
{
    FLAGS = 0,
    FLAGS_DEFINED,	// mask indicating if the flag was specified
    FLAGS2,
    FLAGS_DEFINED2,
    COLOR,
    ALPHA,
    BASETEXTURE,
    FRAME,
    BASETEXTURETRANSFORM,
    FLASHLIGHTTEXTURE,
    FLASHLIGHTTEXTUREFRAME,
    COLOR2,
    SRGBTINT,

    NUM_SHADER_MATERIAL_VARS
};

class IMaterial
{
public:
    VFUNC(const char*, getName, 0, (), (this));
    VFUNC(const char*, getTextureGroupName, 1, (), (this));
    VFUNC(IMaterialVar*, findVar, 11, (const char* name, bool* found = nullptr, bool complain = true), (this, name, found, complain));
    VFUNC(void, alphaModulate, 27, (float alpha), (this, alpha));
    VFUNC(void, alphaModulate, 27, (const Color& color), (this, color.a()));
    VFUNC(void, colorModulate, 28, (float r, float g, float b), (this, r, g, b));
    VFUNC(void, colorModulate, 28, (const Color& color), (this, color.r(), color.g(), color.b()));
    VFUNC(void, setMaterialVarFlag, 29, (MaterialVarFlags_t flag, bool on), (this, flag, on));
    VFUNC(void, refresh, 37, (), (this));
    VFUNC(IMaterialVar**, getShaderParams, 41, (), (this));
    VFUNC(bool, isError, 42, (), (this));
    VFUNC(void, addRefCount, 12, (), (this));
    VFUNC(void, removeRefCount, 13, (), (this));
    VFUNC(float, getAlphaModulation, 44, (), (this));
    VFUNC(void, getColorModulation, 45, (float* r, float* g, float* b), (this, r, g, b));
    VFUNC(void, refreshPreservingMaterialVars, 53, (), (this));

    // goes for rgba
    void modulateAllColor(const Color& color)
    {
        this->colorModulate(color);
        this->alphaModulate(color);
    }

    void destroy();
    // crashing
    bool precacheVars(void* vmtValues = nullptr, void* patchValues = nullptr, void* includes = nullptr);

    PAD(124);
    KeyValues* m_keyValues;
};


class IMaterialVar
{
public:
    VFUNC(void, setValue, 4, (float val), (this, val));
    VFUNC(void, setValue, 5, (int val), (this, val));
    VFUNC(void, setValue, 6, (const char* val), (this, val));
    VFUNC(void, setValues, 11, (const Vec3& val), (this, val[0], val[1], val[2]));
    VFUNC(void, setValues, 11, (const Color& val), (this, val.r(), val.g(), val.b()));
    VFUNC(void, setValues, 11, (float x, float y, float z), (this, x, y, z));
    VFUNC(void, setVectorComponent, 26, (float val, int component), (this, val, component));

    VFUNC(const char*, getString, 7, (), (this));
    VFUNC(int, getInt, 27, (), (this));
    VFUNC(const float*, getVector, 30, (), (this));
};