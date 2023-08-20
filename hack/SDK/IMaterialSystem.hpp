#pragma once

#include "material.hpp"
#include "Enums.hpp"
#include "vars.hpp"

#include "helpers/vfunc.hpp"
#include <gamememory/memory.hpp>

class IMaterial;
class KeyValues;
class IMatRenderContext;
class ITexture;

// we can make some leak, but enabling the allocation here might make a lag
// game inits it always before the game, not a big deal to just see an error
//#define DONT_IGNORE_MATSYS_ALLOCATION_ERRORS

class IMaterialSystem
{
public:
	VFUNC(IMaterial*, createMaterial, 83, (const char* materialName, KeyValues* keyValues), (this, materialName, keyValues));
	VFUNC(IMaterial*, findMaterial, 84, (char const* name, const char* group, bool complain = true, const char* prefix = nullptr), (this, name, group, complain, prefix));
	VFUNC(short, firstMaterial, 86, (), (this));
	VFUNC(short, nextMaterial, 87, (int handle), (this, handle));
	VFUNC(short, invalidMaterialFromHandle, 88, (), (this));
	VFUNC(IMaterial*, getMaterial, 89, (short handle), (this, handle));
	VFUNC(int, getMaterialCount, 90, (), (this));
	VFUNC(ITexture*, findTexture, 91, (const char* name, const char* group, bool complain = true, int additionalFlags = 0),
		(this, name, group, complain, additionalFlags));
	VFUNC(IMatRenderContext*, getRenderContext, 115, (), (this));
	VFUNC(void, beginRenderTargetAllocation, 94, (), (this));
	VFUNC(void, endRenderTargetAllocation, 95, (), (this));
	VFUNC(ITexture*, createNamedRenderTargetTextureEx, 97,
		(const char* name, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format,
			MaterialRenderTargetDepth_t depth, unsigned int textureFlags, unsigned int renderTargetFlags),
		(this, name, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags));

	ITexture* createFullFrameRenderTarget(const char* name)
	{
		return createNamedRenderTargetTextureEx(
			name, 1, 1, RT_SIZE_FULL_FRAME_BUFFER,
			IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
			CREATERENDERTARGETFLAGS_HDR);
	}
	VFUNC(ImageFormat, getBackBufferFormat, 36, (), (this));

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/234226-mirrorcam-ported-window.html

	void beginRenderTargetAllocationEx()
	{
#if defined DONT_IGNORE_MATSYS_ALLOCATION_ERRORS
		bool old = m_disableRenderTargetAllocationForever;
		m_disableRenderTargetAllocationForever = false;
#endif
		beginRenderTargetAllocation();
#if defined DONT_IGNORE_MATSYS_ALLOCATION_ERRORS
		m_disableRenderTargetAllocationForever = old;
#endif
	}

	void endRenderTargetAllocationEx()
	{
#if defined DONT_IGNORE_MATSYS_ALLOCATION_ERRORS
		bool old = m_disableRenderTargetAllocationForever;
		m_disableRenderTargetAllocationForever = false;
#endif
		endRenderTargetAllocation();
#if defined DONT_IGNORE_MATSYS_ALLOCATION_ERRORS
		m_disableRenderTargetAllocationForever = old;
#endif
	}

	void forceSingleThreaded()
	{
		memory::forceSingleThreaded()(this);
	}

	VFUNC(bool, allowThreading, 138, (bool enabled, int thread), (this, enabled, thread));

	PAD(11288);
	bool m_disableRenderTargetAllocationForever;
	PAD(40);
};