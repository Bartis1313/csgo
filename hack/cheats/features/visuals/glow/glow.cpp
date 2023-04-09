#include "glow.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/IVStudioRender.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/IViewRender.hpp>
#include <SDK/IVRenderView.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/ITexture.hpp>
#include <SDK/CGlowManager.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/stencil.hpp>
#include <SDK/PixEvent.hpp>
#include <SDK/Enums.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/materialInit.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <cheats/features/cache/cache.hpp>
#include <config/vars.hpp>
#include <cheats/features/visuals/chams/factory/create.hpp>

// recreation of: https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/glow_outline_effect.cpp

static void setRenderTargetAndViewPort(ITexture* rt, int w, int h)
{
	const auto ctx = memory::interfaces::matSys->getRenderContext();
	ctx->setRenderTarget(rt);
	ctx->viewport(0, 0, w, h);
}

void Glow::initMaterials()
{
	_rt_FullFrameFB = memory::interfaces::matSys->findTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);
	_rt_FullScreen = memory::interfaces::matSys->findTexture("_rt_FullScreen", TEXTURE_GROUP_RENDER_TARGET);
	_rt_SmallFB0 = memory::interfaces::matSys->findTexture("_rt_SmallFB0", TEXTURE_GROUP_RENDER_TARGET);
	_rt_SmallFB1 = memory::interfaces::matSys->findTexture("_rt_SmallFB1", TEXTURE_GROUP_RENDER_TARGET);


	glow_color = memory::interfaces::matSys->findMaterial("dev/glow_color", TEXTURE_GROUP_OTHER);
	glow_downsample = material::factory::createMaterial(MaterialData{ .name = "downsampleGlow_fixed", .key = "Downsample_nohdr",
		.buffer = "$bloomtintenable 1 $bloomtype 1 $basetexture _rt_FullFrameFB $bloomexp 2.5 $bloomsaturation 1.0" }, CreationType::FROM_STRING);
	glow_rim3d = memory::interfaces::matSys->findMaterial("dev/glow_rim3d", TEXTURE_GROUP_OTHER);
	glow_blur_x = memory::interfaces::matSys->findMaterial("dev/glow_blur_x", TEXTURE_GROUP_OTHER);
	glow_blur_y = memory::interfaces::matSys->findMaterial("dev/glow_blur_y", TEXTURE_GROUP_OTHER);
	halo_add_to_screen = memory::interfaces::matSys->findMaterial("dev/halo_add_to_screen", TEXTURE_GROUP_OTHER);
	glow_edge_highlight = memory::interfaces::matSys->findMaterial("dev/glow_edge_highlight", TEXTURE_GROUP_OTHER);

	console::debug("loaded all glow textures / materials");
}

void Glow::downSampleAndBlurRT(IMatRenderContext* ctx)
{
	ctx->pushRenderTargetAndViewport();

	const int srcWidth = globals::screenX;
	const int srcHeight = globals::screenY;

	if ((_rt_SmallFB0->getActualWidth() != (globals::screenX / 4)) || (_rt_SmallFB0->getActualHeight() != (globals::screenY / 4)))
	{
		ctx->setRenderTarget(_rt_SmallFB0);
		ctx->viewport(0, 0, _rt_SmallFB0->getActualWidth(), _rt_SmallFB0->getActualHeight());
		ctx->clearColor3ub(0, 0, 0);
		ctx->clearBuffers(true, false, false);
	}

	ctx->setRenderTarget(_rt_SmallFB0);
	ctx->viewport(0, 0, globals::screenX / 4, globals::screenY / 4);

	if (const auto var = glow_downsample->findVar("$bloomexp"))
		var->setValue(vars::visuals->glow->exponent);

	if (const auto var = glow_downsample->findVar("$bloomsaturation"))
		var->setValue(vars::visuals->glow->saturation);

	const int fullFbWidth = srcWidth;
	const int fullFbHeight = srcHeight;

	ctx->drawScreenSpaceRectangle(glow_downsample, 0, 0, srcWidth / 4, srcHeight / 4,
		0, 0, fullFbWidth - 4.0f, fullFbHeight - 4.0f,
		_rt_FullFrameFB->getActualWidth(), _rt_FullFrameFB->getActualHeight(), nullptr, 0, 0);

	static bool first = true;
	if (first || (_rt_SmallFB1->getActualWidth() != (globals::screenX / 4)) || (_rt_SmallFB1->getActualHeight() != (globals::screenY / 4)))
	{
		first = false;
		ctx->setRenderTarget(_rt_SmallFB1);
		ctx->viewport(0, 0, _rt_SmallFB1->getActualWidth(), _rt_SmallFB1->getActualHeight());
		ctx->clearColor3ub(0, 0, 0);
		ctx->clearBuffers(true, false, false);
	}

	ctx->setRenderTarget(_rt_SmallFB1);
	ctx->viewport(0, 0, globals::screenX / 4, globals::screenY / 4);

	ctx->drawScreenSpaceRectangle(glow_blur_x, 0, 0, srcWidth / 4, srcHeight / 4,
		0, 0, srcWidth / 4.0f - 1.0f, srcHeight / 4.0f - 1.0f,
		_rt_SmallFB0->getActualWidth(), _rt_SmallFB0->getActualHeight(), nullptr, 0, 0);

	ctx->setRenderTarget(_rt_SmallFB0);
	ctx->viewport(0, 0, globals::screenX / 4, globals::screenY / 4);

	if (const auto var = glow_blur_y->findVar("$bloomamount"))
		var->setValue(vars::visuals->glow->thickness);

	ctx->drawScreenSpaceRectangle(glow_blur_y, 0, 0, srcWidth / 4, srcHeight / 4,
		0, 0, srcWidth / 4.0f - 1.0f, srcHeight / 4.0f - 1.0f,
		_rt_SmallFB1->getActualWidth(), _rt_SmallFB1->getActualHeight(), nullptr, 0, 0);

	ctx->popRenderTargetAndViewport();
}

void Glow::renderGlowModels(IMatRenderContext* ctx, int x, int y)
{
	ctx->pushRenderTargetAndViewport();

	auto originalColor = Colors::Blank.getRGB();
	memory::interfaces::renderView->getcolor(originalColor.data());
	const float originalBlend = memory::interfaces::renderView->getBlend();

	setRenderTargetAndViewPort(_rt_FullFrameFB, x, y);

	ctx->clearColor3ub(0, 0, 0);
	ctx->clearBuffers(true, false, false);

	glow_color->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, vars::visuals->glow->ignorez.at(E2T(GlowRenderStyle::DEFAULT)));
	memory::interfaces::studioRender->forcedMaterialOverride(glow_color);

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		if (ent->isDormant())
			continue;

		if (!ent->isOtherTeam(game::localPlayer()))
			continue;

		memory::interfaces::renderView->setBlend(vars::visuals->glow->colorPlayer().a());
		memory::interfaces::renderView->modulateColor(vars::visuals->glow->colorPlayer().data());
		ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
	}

	memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
	memory::interfaces::renderView->modulateColor(originalColor.data());
	memory::interfaces::renderView->setBlend(originalBlend);

	ShaderStencilState_t stencilStateDisable;
	stencilStateDisable.m_bEnable = false;
	ctx->setStencilState(stencilStateDisable);

	ctx->popRenderTargetAndViewport();
}

void Glow::applyEntityGlowEffects(IMatRenderContext* ctx)
{
	std::vector<Player_t*> rim3DGlows, edgeHighlightGlows, glows;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		if (ent->isDormant())
			continue;

		if (!ent->isOtherTeam(game::localPlayer()))
			continue;

		if (const auto cfg = vars::visuals->glow->usedMats; cfg.at(E2T(GlowRenderStyle::RIMGLOW3D)))
		{
			rim3DGlows.push_back(ent);
		}
		else if (cfg.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT)) || cfg.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT_PULSE)))
		{
			edgeHighlightGlows.push_back(ent);
		}

		glows.push_back(ent);
	}

	if (rim3DGlows.size())
	{
		const float pulse = 0.5f + 0.5f * std::sin(memory::interfaces::globalVars->m_curtime * vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::RIM)));

		memory::interfaces::studioRender->forcedMaterialOverride(glow_rim3d);
		glow_rim3d->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, vars::visuals->glow->ignorez.at(E2T(GlowRenderStyle::RIMGLOW3D)));

		for (const auto& ent : rim3DGlows)
		{
			if (vars::visuals->glow->colorPlayer().a() <= 0.0f)
				continue;

			if (const auto var = glow_rim3d->findVar("$envmaptint"))
			{
				var->setVectorComponent(std::clamp(vars::visuals->glow->colorPlayer().a() * vars::visuals->glow->colorPlayer().r(), 0.0f, 1.0f), 0);
				var->setVectorComponent(std::clamp(vars::visuals->glow->colorPlayer().a() * vars::visuals->glow->colorPlayer().g(), 0.0f, 1.0f), 1);
				var->setVectorComponent(std::clamp(vars::visuals->glow->colorPlayer().a() * vars::visuals->glow->colorPlayer().b(), 0.0f, 1.0f), 2);
			}

			if (const auto var = glow_rim3d->findVar("$envmapfresnelminmaxexp"))
			{
				var->setVectorComponent(0.0f, 0);
				var->setVectorComponent(1.5f, 1);
				var->setVectorComponent(3.0f + pulse, 2);
			}

			ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
		}

		memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
	}

	if (edgeHighlightGlows.size())
	{
		ctx->pushRenderTargetAndViewport();

		auto originalColor = Colors::Blank.getRGB();
		memory::interfaces::renderView->getcolor(originalColor.data());

		setRenderTargetAndViewPort(_rt_FullScreen, globals::screenX, globals::screenY);

		ctx->clearColor3ub(0, 0, 0);
		ctx->clearBuffers(true, true, true);

		memory::interfaces::studioRender->forcedMaterialOverride(glow_color);

		memory::interfaces::renderView->setBlend(1.0f);

		auto overrideColor = Colors::White.getRGB();
		memory::interfaces::renderView->modulateColor(overrideColor.data());

		for (const auto& ent : edgeHighlightGlows)
			ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());

		memory::interfaces::studioRender->forcedMaterialOverride(nullptr);

		ctx->popRenderTargetAndViewport();

		memory::interfaces::studioRender->forcedMaterialOverride(glow_color);

		ctx->overrideDepthEnable(true, false);

		glow_color->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ,
			vars::visuals->glow->ignorez.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT))
			|| vars::visuals->glow->ignorez.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT_PULSE)));

		for (const auto& ent : edgeHighlightGlows)
		{
			Vec3 vecTempColor = Vec3{ vars::visuals->glow->colorPlayer().getRGB() } *vars::visuals->glow->colorPlayer().a() * 1.4f;

			if (vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT_PULSE)))
			{
				const float pulse = 1.5f + 0.5f * std::sin(memory::interfaces::globalVars->m_curtime * vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::EDGE)));
				vecTempColor *= (pulse * 0.5f);
			}

			memory::interfaces::renderView->modulateColor(vecTempColor.toArray().data());

			ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
		}

		memory::interfaces::renderView->modulateColor(originalColor.data());
	}

	memory::interfaces::studioRender->forcedMaterialOverride(glow_color);
	const float originalBlend = memory::interfaces::renderView->getBlend();

	memory::interfaces::renderView->setBlend(0.0f);
	int glowObjects = 0;

	for (const auto& ent : glows)
	{
		if (!vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::DEFAULT)))
			continue;

		if (vars::visuals->glow->fullBloom)
		{
			++glowObjects;
			continue;
		}

		if (vars::visuals->glow->occluded || vars::visuals->glow->unoccluded)
		{
			if (vars::visuals->glow->occluded && vars::visuals->glow->unoccluded)
			{
				ShaderStencilState_t stencilState;
				stencilState.m_bEnable = true;
				stencilState.m_nReferenceValue = 1;
				stencilState.m_CompareFunc = SHADER_STENCILFUNC_ALWAYS;
				stencilState.m_PassOp = SHADER_STENCILOP_SET_TO_REFERENCE;
				stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
				stencilState.m_ZFailOp = SHADER_STENCILOP_SET_TO_REFERENCE;

				ctx->setStencilState(stencilState);

				ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
			}
			else if (vars::visuals->glow->occluded)
			{
				ShaderStencilState_t stencilState;
				stencilState.m_bEnable = true;
				stencilState.m_nReferenceValue = 1;
				stencilState.m_CompareFunc = SHADER_STENCILFUNC_ALWAYS;
				stencilState.m_PassOp = SHADER_STENCILOP_KEEP;
				stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
				stencilState.m_ZFailOp = SHADER_STENCILOP_SET_TO_REFERENCE;

				ctx->setStencilState(stencilState);

				ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
			}
			else if (vars::visuals->glow->unoccluded)
			{
				ShaderStencilState_t stencilState;
				stencilState.m_bEnable = true;
				stencilState.m_nReferenceValue = 2;
				stencilState.m_nTestMask = 0x1;
				stencilState.m_nWriteMask = 0x3;
				stencilState.m_CompareFunc = SHADER_STENCILFUNC_EQUAL;
				stencilState.m_PassOp = SHADER_STENCILOP_INCREMENT_CLAMP;
				stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
				stencilState.m_ZFailOp = SHADER_STENCILOP_SET_TO_REFERENCE;

				ctx->setStencilState(stencilState);

				ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
			}
		}

		glowObjects++;
	}

	// 2nd pass

	for (const auto& ent : glows)
	{
		if (!vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::DEFAULT)))
			continue;

		if (vars::visuals->glow->fullBloom)
			continue;

		if (vars::visuals->glow->occluded && !vars::visuals->glow->unoccluded)
		{
			ShaderStencilState_t stencilState;
			stencilState.m_bEnable = true;
			stencilState.m_nReferenceValue = 2;
			stencilState.m_CompareFunc = SHADER_STENCILFUNC_ALWAYS;
			stencilState.m_PassOp = SHADER_STENCILOP_SET_TO_REFERENCE;
			stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
			stencilState.m_ZFailOp = SHADER_STENCILOP_KEEP;

			ctx->setStencilState(stencilState);
		}

		ent->drawModel(renderFlags, vars::visuals->glow->colorPlayer().aMultiplied());
	}

	// skipped all glowboxes stuff
	// skipped all health stuff

	ctx->overrideDepthEnable(false, false);
	memory::interfaces::renderView->setBlend(originalBlend);
	ShaderStencilState_t stencilStateDisable;
	stencilStateDisable.m_bEnable = false;
	ctx->setStencilState(stencilStateDisable);
	memory::interfaces::studioRender->forcedMaterialOverride(nullptr);

	if (glowObjects <= 0)
		return;

	PIXEVENT{ ctx, "RenderGlowModels" };
	renderGlowModels(ctx, globals::screenX, globals::screenY);

	downSampleAndBlurRT(ctx);

	if (const auto var = halo_add_to_screen->findVar("$C0_X"))
		var->setValue(vars::visuals->glow->C0_X);

	ShaderStencilState_t stencilState;
	stencilState.m_bEnable = true;
	stencilState.m_nWriteMask = 0x0; // We're not changing stencil
	//stencilState.m_nTestMask = 0x3;
	stencilState.m_nReferenceValue = 0x0;
	stencilState.m_CompareFunc = SHADER_STENCILFUNC_EQUAL;
	stencilState.m_PassOp = SHADER_STENCILOP_KEEP;
	stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
	stencilState.m_ZFailOp = SHADER_STENCILOP_ZERO;
	ctx->setStencilState(stencilState);

	const int srcWidth = globals::screenX;
	const int srcHeight = globals::screenY;
	int viewportX, viewportY, viewportW, viewportH;
	ctx->getViewport(viewportX, viewportY, viewportW, viewportH);

	ctx->drawScreenSpaceRectangle(halo_add_to_screen, 0, 0, srcWidth, srcHeight,
		0.0f, -0.5f, srcWidth / 4.0f - 1.0f, srcHeight / 4.0f - 1.0f,
		_rt_SmallFB1->getActualWidth(),
		_rt_SmallFB1->getActualHeight(), nullptr, 0, 0);

	ctx->setStencilState(stencilStateDisable);
}

void Glow::run()
{
	INIT_MATERIALS_ONCE(initMaterials);

	if (!vars::visuals->glow->enabled)
		return;

	if (!game::isAvailable())
		return;

	auto ctx = memory::interfaces::matSys->getRenderContext();

	PIXEVENT{ ctx, "EntityGlowEffects" };
	applyEntityGlowEffects(ctx);
}