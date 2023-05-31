#include "glow.hpp"

#include "../chams/factory/factory.hpp"

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
#include <utilities/math/math.hpp>
#include <render/render.hpp>
#include <cheats/features/visuals/player/boxes.hpp>

#include <cheats/hooks/doPostScreenEffects.hpp>

#include "../streamproof/streamproof.hpp"

// rebuild of: https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/glow_outline_effect.cpp
// 1 - less drawModel calls
// 2 - no occluded and unonccluded confusing shit
// 3 - full bloom removed - useless
// 4 - visible / zbuffer chams
// 5 - actual blurred lines
// 6 - THANK YOU tlxkjc2407 for the glow visibility

namespace
{
	struct GlowHandler : hooks::DoPostScreenEffects
	{
		GlowHandler()
		{
			this->registerInit(glow::initMaterials);
			this->registerRun(glow::run);
		}
	} glowHandler;
}

#define GLOWBOX_PASS_COLOR 0
#define GLOWBOX_PASS_STENCIL 1

namespace glow
{
	struct GlowBoxDefinition_t
	{
		Vec3 m_position;
		Vec3 m_angOrientation;
		Vec3 m_mins;
		Vec3 m_maxs;
		float m_birthTimeIndex;
		float m_terminationTimeIndex; //when to die
		Color m_color;
	};

	void downSampleAndBlurRT(IMatRenderContext* ctx);
	void renderGlowModels(IMatRenderContext* ctx);
	void renderGlowBoxes(int pass, IMatRenderContext* ctx);
	void renderGlowMisc(IMatRenderContext* ctx);
	void beginGlow(IMatRenderContext* ctx);
	void endGlow(IMatRenderContext* ctx);
	void addBackbufferGlow(IMatRenderContext* ctx);
	void addHaloScreen(IMatRenderContext* ctx);
	void applyEntityGlowEffects(IMatRenderContext* ctx);
	void drawModel(Player_t* ent, float alpha);

	ITexture* _rt_FullFrameFB{ };
	ITexture* _rt_FullFrameFB1{ };
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
	IMaterial* __utilVertexColor{ };
	IMaterial* __utilVertexColorIgnoreZ{ };
	IMaterial* debugfbtexture1{ };

	std::array<float, 3> originalColorBegin;
	float originalBlendBegin;

	std::vector<GlowBoxDefinition_t> glowBoxDefinitions{ };

	constexpr int renderFlags{ STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS };
}

static void setRenderTargetAndViewPort(ITexture* rt, int w, int h)
{
	const auto ctx = memory::interfaces::matSys->getRenderContext();
	ctx->setRenderTarget(rt);
	ctx->viewport(0, 0, w, h);
}

static void renderBox(const Vec3& origin, const Vec3& angles, const Vec3& mins, const Vec3& maxs, const Color& color, bool zBuffer, bool insideOut)
{
	IMaterial* mat = zBuffer ? glow::__utilVertexColor : glow::__utilVertexColorIgnoreZ;

	// added this because alpha didn't seem to change, I missed something
	// tho, this is fairly enough
	mat->alphaModulate(color.a());

	memory::renderBoxInternal()(origin, angles, mins, maxs, SDKColor{ color }, mat, insideOut);
}

static void renderLine(const Vec3& start, const Vec3& end, const Color& color, bool zBuffer)
{
	memory::renderLine()(start, end, SDKColor{ color }, zBuffer);
}

void glow::initMaterials()
{
	_rt_FullFrameFB = material::factory::findTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);
	_rt_FullFrameFB1 = material::factory::findTexture("_rt_FullFrameFB1", TEXTURE_GROUP_RENDER_TARGET);
	_rt_FullScreen = material::factory::findTexture("_rt_FullScreen", TEXTURE_GROUP_RENDER_TARGET);
	_rt_SmallFB0 = material::factory::findTexture("_rt_SmallFB0", TEXTURE_GROUP_RENDER_TARGET);
	_rt_SmallFB1 = material::factory::findTexture("_rt_SmallFB1", TEXTURE_GROUP_RENDER_TARGET);

	glow_color = material::factory::findMaterial("dev/glow_color", TEXTURE_GROUP_OTHER);
	MaterialData downSampleFixed
	{
		.name = "downsampleGlow_fixed",
		.key = "Downsample_nohdr",
		.buffer = "$bloomtintenable 1 $bloomtype 1 $basetexture _rt_FullFrameFB $bloomexp 2.5 $bloomsaturation 1.0 $noToolTexture 1",
		.createType = CreationType::FROM_STRING
	};
	glow_downsample = material::factory::createMaterial(downSampleFixed);
	glow_rim3d = material::factory::findMaterial("dev/glow_rim3d", TEXTURE_GROUP_OTHER);
	glow_blur_x = material::factory::findMaterial("dev/glow_blur_x", TEXTURE_GROUP_OTHER);
	glow_blur_y = material::factory::findMaterial("dev/glow_blur_y", TEXTURE_GROUP_OTHER);
	halo_add_to_screen = material::factory::findMaterial("dev/halo_add_to_screen", TEXTURE_GROUP_OTHER);
	glow_edge_highlight = material::factory::findMaterial("dev/glow_edge_highlight", TEXTURE_GROUP_OTHER);
	MaterialData utilVertex
	{
		.name = "__utilVertex",
		.key = "unlitgeneric",
		.buffer = "$vertexcolor 1 vertexalpha 1",
		.createType = CreationType::FROM_STRING
	};
	__utilVertexColor = material::factory::createMaterial(utilVertex);
	MaterialData utilVertexIgnorez
	{
		.name = "__utilVertex",
		.key = "unlitgeneric",
		.buffer = "$vertexcolor 1 vertexalpha 1 $ignorez 1",
		.createType = CreationType::FROM_STRING
	};
	__utilVertexColorIgnoreZ = material::factory::createMaterial(utilVertexIgnorez);
	MaterialData debugmat
	{
		.name = "debugfbtexture1",
		.key = "UnlitGeneric",
		.buffer = "$basetexture _rt_FullFrameFB1 $noToolTexture 1",
		.createType = CreationType::FROM_STRING
	};
	debugfbtexture1 = material::factory::createMaterial(debugmat);

	streamProof.init();

	console::debug("loaded all glow textures / materials");
}

void glow::downSampleAndBlurRT(IMatRenderContext* ctx)
{
	const int srcWidth = globals::screenX;
	const int srcHeight = globals::screenY;

	if ((_rt_SmallFB0->getActualWidth() != (globals::screenX / 4)) || (_rt_SmallFB0->getActualHeight() != (globals::screenY / 4)))
	{
		setRenderTargetAndViewPort(_rt_SmallFB0, _rt_SmallFB0->getActualWidth(), _rt_SmallFB0->getActualHeight());
		ctx->clearColor3ub(0, 0, 0);
		ctx->clearBuffers(true, false);
	}

	setRenderTargetAndViewPort(_rt_SmallFB0, globals::screenX / 4, globals::screenY / 4);

	if (const auto var = glow_downsample->findVar("$bloomexp"))
		var->setValue(vars::visuals->glow->exponent);

	if (const auto var = glow_downsample->findVar("$bloomsaturation"))
		var->setValue(vars::visuals->glow->saturation);

	const int fullFbWidth = srcWidth;
	const int fullFbHeight = srcHeight;

	ctx->drawScreenSpaceRectangle(glow_downsample, 0, 0, srcWidth / 4, srcHeight / 4,
		0, 0, fullFbWidth - 4.0f, fullFbHeight - 4.0f,
		_rt_FullFrameFB->getActualWidth(), _rt_FullFrameFB->getActualHeight());

	static bool first = true;
	if (first || (_rt_SmallFB1->getActualWidth() != (globals::screenX / 4)) || (_rt_SmallFB1->getActualHeight() != (globals::screenY / 4)))
	{
		first = false;
		setRenderTargetAndViewPort(_rt_SmallFB1, _rt_SmallFB1->getActualWidth(), _rt_SmallFB1->getActualHeight());
		ctx->clearColor3ub(0, 0, 0);
		ctx->clearBuffers(true, false);
	}

	setRenderTargetAndViewPort(_rt_SmallFB1, globals::screenX / 4, globals::screenY / 4);

	ctx->drawScreenSpaceRectangle(glow_blur_x, 0, 0, srcWidth / 4, srcHeight / 4,
		0, 0, srcWidth / 4.0f - 1.0f, srcHeight / 4.0f - 1.0f,
		_rt_SmallFB0->getActualWidth(), _rt_SmallFB0->getActualHeight());

	setRenderTargetAndViewPort(_rt_SmallFB0, globals::screenX / 4, globals::screenY / 4);

	if (const auto var = glow_blur_y->findVar("$bloomamount"))
		var->setValue(vars::visuals->glow->thickness);

	ctx->drawScreenSpaceRectangle(glow_blur_y, 0, 0, srcWidth / 4, srcHeight / 4,
		0, 0, srcWidth / 4.0f - 1.0f, srcHeight / 4.0f - 1.0f,
		_rt_SmallFB1->getActualWidth(), _rt_SmallFB1->getActualHeight());
}

void glow::renderGlowModels(IMatRenderContext* ctx)
{
	if (!vars::visuals->glow->enabled)
		return;

	if (!vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::DEFAULT)))
		return;

	ShaderStencilState_t stencilState;
	stencilState.m_bEnable = true;
	stencilState.m_nReferenceValue = 1;
	stencilState.m_CompareFunc = SHADER_STENCILFUNC_ALWAYS;
	stencilState.m_PassOp = SHADER_STENCILOP_SET_TO_REFERENCE;
	stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
	stencilState.m_ZFailOp = SHADER_STENCILOP_SET_TO_REFERENCE;

	ctx->setStencilState(stencilState);

	ctx->overrideDepthEnable(vars::visuals->glow->visible, false);
	memory::interfaces::renderView->setBlend(vars::visuals->glow->colorPlayer().a());

	for (auto [ent, idx, classid] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		const auto player = ent->cast<Player_t*>();

		if (player->isDormant())
			continue;

		if (!player->isOtherTeam(game::localPlayer()))
			continue;

		memory::interfaces::renderView->modulateColor(vars::visuals->glow->colorPlayer().data());
		drawModel(player, vars::visuals->glow->colorPlayer().a());
	}
}

void glow::renderGlowBoxes(int pass, IMatRenderContext* ctx)
{
	for (size_t i = 0; auto & glowBox : glowBoxDefinitions)
	{
		if (glowBox.m_terminationTimeIndex < memory::interfaces::globalVars->m_curtime)
		{
			glowBoxDefinitions.erase(glowBoxDefinitions.begin() + i);
			continue;
		}

		float lifeLeft = (glowBox.m_terminationTimeIndex - memory::interfaces::globalVars->m_curtime) /
			(glowBox.m_terminationTimeIndex - glowBox.m_birthTimeIndex);
		if (lifeLeft > 0.95f)
			lifeLeft = (0.05f - (lifeLeft - 0.95f)) / 0.05f; // fade in the first 5% of lifetime
		else
			lifeLeft = std::min(lifeLeft * 4.0f, 1.0f); // fade out the last 25% of lifetime

		glowBox.m_color.a() = lifeLeft;

		if (pass == GLOWBOX_PASS_COLOR)
		{
			const Vec3 forward = math::angleVec(glowBox.m_angOrientation);
			const Vec3 lineEnd = glowBox.m_position + (forward * glowBox.m_mins[0]);

			renderLine(glowBox.m_position, lineEnd, glowBox.m_color, false);
		}
		else if (pass == GLOWBOX_PASS_STENCIL)
		{
			ShaderStencilState_t stencilState;
			stencilState.m_bEnable = true;
			stencilState.m_nReferenceValue = 1;
			stencilState.m_CompareFunc = SHADER_STENCILFUNC_ALWAYS;
			stencilState.m_PassOp = SHADER_STENCILOP_KEEP;
			stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
			stencilState.m_ZFailOp = SHADER_STENCILOP_KEEP;

			ctx->setStencilState(stencilState);

			renderBox(glowBox.m_position, glowBox.m_angOrientation, glowBox.m_mins, glowBox.m_maxs, glowBox.m_color, false, false);
		}

		++i;
	}
}

void glow::renderGlowMisc(IMatRenderContext* ctx)
{
	if (!vars::visuals->glow->enabled)
		return;

	if (std::ranges::all_of(vars::visuals->glow->usedMats, [](const bool b) { return !b; }))
		return;

	std::vector<Player_t*> rim3DGlows, edgeHighlightGlows;

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
	}

	if (rim3DGlows.size())
	{
		const float pulse = 0.5f + 0.5f * std::sin(memory::interfaces::globalVars->m_curtime * vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::RIM)));

		memory::interfaces::studioRender->forcedMaterialOverride(glow_rim3d);
		glow_rim3d->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, vars::visuals->glow->ignorez.at(E2T(GlowIgnorez::RIMGLOW3D)));

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

			drawModel(ent, vars::visuals->glow->colorPlayer().a());
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
			drawModel(ent, vars::visuals->glow->colorPlayer().a());

		memory::interfaces::studioRender->forcedMaterialOverride(nullptr);

		ctx->popRenderTargetAndViewport();

		memory::interfaces::studioRender->forcedMaterialOverride(glow_edge_highlight);

		ctx->overrideDepthEnable(true, false);

		glow_edge_highlight->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ,
			vars::visuals->glow->ignorez.at(E2T(GlowIgnorez::EDGE_HIGHLIGHT))
			|| vars::visuals->glow->ignorez.at(E2T(GlowIgnorez::EDGE_HIGHLIGHT_PULSE)));

		for (const auto& ent : edgeHighlightGlows)
		{
			Vec3 vecTempColor = Vec3{ vars::visuals->glow->colorPlayer().getRGB() } *vars::visuals->glow->colorPlayer().a() * 1.4f;

			if (vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT_PULSE)))
			{
				const float pulse = 1.5f + 0.5f * std::sin(memory::interfaces::globalVars->m_curtime * vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::EDGE)));
				vecTempColor *= (pulse * 0.5f);
			}

			memory::interfaces::renderView->modulateColor(vecTempColor.toArray().data());

			drawModel(ent, vars::visuals->glow->colorPlayer().a());
		}

		memory::interfaces::renderView->modulateColor(originalColor.data());
	}
}

void glow::beginGlow(IMatRenderContext* ctx)
{
	ctx->copyRenderTargetToTexture(_rt_FullFrameFB1);
	ctx->clearColor3ub(0, 0, 0);
	ctx->clearBuffers(true, false);

	originalColorBegin = Colors::Blank.getRGB();
	memory::interfaces::renderView->getcolor(originalColorBegin.data());
	originalBlendBegin = memory::interfaces::renderView->getBlend();

	memory::interfaces::studioRender->forcedMaterialOverride(glow_color);
}

void glow::endGlow(IMatRenderContext* ctx)
{
	memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
	memory::interfaces::renderView->modulateColor(originalColorBegin.data());
	memory::interfaces::renderView->setBlend(originalBlendBegin);

	ctx->overrideDepthEnable(false, false);
}

void glow::addBackbufferGlow(IMatRenderContext* ctx)
{
	ctx->copyRenderTargetToTexture(_rt_FullFrameFB);
	ctx->disableStencil();

	const int srcWidth = globals::screenX;
	const int srcHeight = globals::screenY;
	int viewportX, viewportY, viewportW, viewportH;
	ctx->getViewport(viewportX, viewportY, viewportW, viewportH);

	ctx->overrideDepthEnable(true, false);
	ctx->drawScreenSpaceRectangle(debugfbtexture1,
		0, 0, viewportW, viewportH,
		0, 0, srcWidth - 1.0f, srcHeight - 1.0f,
		_rt_FullFrameFB1->getActualWidth(), _rt_FullFrameFB1->getActualHeight());
	ctx->overrideDepthEnable(false, false);
}

void glow::addHaloScreen(IMatRenderContext* ctx)
{
	if (const auto var = halo_add_to_screen->findVar("$C0_X"))
		var->setValue(vars::visuals->glow->C0_X);

	ShaderStencilState_t stencilState;
	stencilState.m_bEnable = true;
	stencilState.m_nWriteMask = 0x1; // We're not changing stencil
	stencilState.m_nReferenceValue = 1;
	stencilState.m_nTestMask = 0x1;
	stencilState.m_CompareFunc = SHADER_STENCILFUNC_NOTEQUAL;
	stencilState.m_PassOp = SHADER_STENCILOP_KEEP;
	stencilState.m_FailOp = SHADER_STENCILOP_KEEP;
	stencilState.m_ZFailOp = SHADER_STENCILOP_KEEP;
	ctx->setStencilState(stencilState);

	const int srcWidth = globals::screenX;
	const int srcHeight = globals::screenY;
	int viewportX, viewportY, viewportW, viewportH;
	ctx->getViewport(viewportX, viewportY, viewportW, viewportH);

	ctx->drawScreenSpaceRectangle(halo_add_to_screen, 0, 0, srcWidth, srcHeight,
		0.0f, -0.5f, srcWidth / 4.0f - 1.0f, srcHeight / 4.0f - 1.0f,
		_rt_SmallFB1->getActualWidth(),
		_rt_SmallFB1->getActualHeight());

	ctx->disableStencil();
}

void glow::applyEntityGlowEffects(IMatRenderContext* ctx)
{
	if (!vars::visuals->glow->enabled && glowBoxDefinitions.empty())
		return;

	renderGlowMisc(ctx);

	ctx->pushRenderTargetAndViewport();

	beginGlow(ctx);
	{
		renderGlowBoxes(GLOWBOX_PASS_STENCIL, ctx);
		renderGlowModels(ctx);
	}
	endGlow(ctx);

	addBackbufferGlow(ctx);

	downSampleAndBlurRT(ctx);

	ctx->popRenderTargetAndViewport();

	addHaloScreen(ctx);
}

void glow::addGlowBox(const Vec3& origin, const Vec3& angOrientation, const Vec3& mins, const Vec3& maxs, const Color& color, float lifetime)
{
#if USE_GLOWBOX_CUSTOM == true
	GlowBoxDefinition_t glowBox
	{
		.m_position = origin,
		.m_angOrientation = angOrientation,
		.m_mins = mins,
		.m_maxs = maxs,
		.m_birthTimeIndex = memory::interfaces::globalVars->m_curtime,
		.m_terminationTimeIndex = memory::interfaces::globalVars->m_curtime + lifetime,
		.m_color = color
	};

	glowBoxDefinitions.push_back(glowBox);
#else
	memory::interfaces::glowManager->addGlowBox(origin, angOrientation, mins, maxs, color, lifetime);
#endif
}

void glow::run()
{
#ifdef USE_GLOW_CUSTOM
	if (!game::isAvailable())
		return;

	auto ctx = memory::interfaces::matSys->getRenderContext();

	applyEntityGlowEffects(ctx);
#else
	// else run it from the interface, careful of register check
#endif
}

void glow::drawModel(Player_t* ent, float alpha)
{
	const uint8_t instanceAlpha = static_cast<uint8_t>(alpha * 255);

	ent->drawModel(renderFlags, instanceAlpha);
	Entity_t* attachment = ent->firstMoveChild();

	while (attachment)
	{
		if (attachment->shouldDraw())
		{
			attachment->drawModel(renderFlags, instanceAlpha);
		}
		attachment = attachment->nextMovePeer();
	}
}