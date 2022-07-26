#pragma once

#include "material.hpp"
#include "../utilities/vfunc.hpp"
#include "../utilities/pad.hpp"
#include "Enums.hpp"
#include "vars.hpp"
#include "../utilities/vfunc.hpp"
#include "structs/IDXandPaterrns.hpp"

class IMaterial;
class KeyValues;
class IMatRenderContext;
class ITexture;

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

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/234226-mirrorcam-ported-window.html

	void forceBeginRenderTargetAllocation()
	{
		bool old = disableRenderTargetAllocationForever();
		disableRenderTargetAllocationForever() = false;
		beginRenderTargetAllocation();
		disableRenderTargetAllocationForever() = old;
	}

	void forceEndRenderTargetAllocation()
	{
		bool old = disableRenderTargetAllocationForever();
		disableRenderTargetAllocationForever() = false;
		endRenderTargetAllocation();
		disableRenderTargetAllocationForever() = old;
	}

	bool& disableRenderTargetAllocationForever()
	{
		const static auto disableTargetAlloc = *reinterpret_cast<uintptr_t*>(utilities::patternScan(MATERIAL_DLL, DISABLE_RENDER_TARGET_ALLOC, 0x2));
		return *reinterpret_cast<bool*>(uintptr_t(this) + disableTargetAlloc);
	}
};
