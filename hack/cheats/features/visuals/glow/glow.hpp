#pragma once

#include <cheats/classes/doPostScreen.hpp>
#include <SDK/vars.hpp>

class IMaterial;
class ITexture;
class IMatRenderContext;

class Glow : protected DoPostScreenType
{
public:
	constexpr Glow() :
		DoPostScreenType{}
	{}

protected:
	virtual void run() override;
private:
	void initMaterials();
	void renderGlowModels(IMatRenderContext* ctx, int x, int y);
	void applyEntityGlowEffects(IMatRenderContext* ctx);
	void downSampleAndBlurRT(IMatRenderContext* ctx);

	ITexture* _rt_FullFrameFB{ };
	ITexture* _rt_FullScreen{ };
	ITexture* _rt_SmallFB0{ };
	ITexture* _rt_SmallFB1{ };

	IMaterial* glow_color{ };
	IMaterial* glow_rim3d{ };
	IMaterial* glow_downsample{ };
	IMaterial* glow_blur_x{ };
	IMaterial* glow_blur_y{ };
	IMaterial* halo_add_to_screen{ };
	IMaterial* glow_edge_highlight{ };

	constexpr static int renderFlags = STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS;
};

GLOBAL_FEATURE(Glow);