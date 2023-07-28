#include "chams.hpp"

#include "editor.hpp"

#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/KeyValuesSys.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IVStudioRender.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/materialInit.hpp>

#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>

#include <cheats/features/cache/cache.hpp>
#include <cheats/hooks/drawModelExecute.hpp>
#include <cheats/helper/initable.hpp>

namespace
{
	struct ChamsHandler : hooks::DrawModelExecute
	{
		ChamsHandler()
		{
			this->registerInit(chams::initMaterials);
		}
	} chamsHandler;

	struct ChamsInit : InitAble
	{
		ChamsInit()
		{
			this->registerInit(chams::init);
		}
	} chamsInit;
}

void chams::init()
{
	MaterialData flat
	{ 
		.name = "Flat",
		.key = "UnlitGeneric",
		.createType = CreationType::FROM_STRING 
	};
	materials.push_back(flat);

	MaterialData _generic
	{ 
		.name = "Generic",
		.key = "VertexLitGeneric",
		.createType = CreationType::FROM_STRING 
	};
	materials.push_back(_generic);

	MaterialData glow
	{	.name = "Glow",
		.key = "VertexLitGeneric",
		.buffer = "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $envmaptint 1",
		.createType = CreationType::FROM_STRING,
		.materialType = MaterialTypes::GLOW
	};
	materials.push_back(glow);

	MaterialData metalic
	{ 
		.name = "Metalic",
		.key = "VertexLitGeneric",
		.buffer = "$basetexture white $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1",
		.createType = CreationType::FROM_STRING,
	};
	materials.push_back(metalic);


	MaterialData pearlescent
	{
		.name = "Pearlescent",
		.key = "VertexLitGeneric",
		.buffer = "$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1",
		.createType = CreationType::FROM_STRING
	};
	materials.push_back(pearlescent);
}

void chams::initMaterials()
{
	for (auto& materialData : materials)
		materialData.material = material::factory::createMaterial(materialData);

	streamProof.init();
	streamProofDME.init();
}

namespace chams::manual
{
	void overrideChams(int styles, bool wireframe, const Color& color);
	void passChams(Player_t* player, CfgCham& cham);
	void drawModel(Player_t* player, bool visible);
	void drawBacktrack(Player_t* player);

	bool shouldRun{ false };
	constexpr int renderFlags = STUDIO_RENDER | STUDIO_DRAWTRANSLUCENTSUBMODELS | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_SKIP_DECALS;
}

namespace chams::DME
{
	enum class Type
	{
		NONE,
		PLAYER,
		LOCAL_WEAPON,
		ARMS
	};

	void callOriginal();

	namespace stored
	{
		IMatRenderContext* ctx;
		DrawModelState_t state;
		ModelRenderInfo_t info;
		Matrix3x4* matrix;
	}

	bool shouldDisable{ false };
}

#pragma region DME
void chams::DME::callOriginal()
{
	hooks::DrawModelExecute::getOriginal()(memory::interfaces::modelRender(), stored::ctx, stored::state, stored::info, stored::matrix);
}

bool chams::DME::shouldDisableDME(const ModelRenderInfo_t& info)
{
	if (!vars::visuals->chams->players.enabled)
		return false;

	const auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(info.m_entIndex));
	if (ent && ent->isPlayer() && ent->isOtherTeam(game::localPlayer()))
	{
		if (vars::visuals->chams->streamProof)
		{
			// HACK, trick the render streamproof to render something
			//memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
			callOriginal();
		}

		return true;
	}

	return false;
}

bool chams::DME::run(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	stored::ctx = ctx;
	stored::state = state;
	stored::info = info;
	stored::matrix = matrix;

	chams::streamProofDME.setActive(vars::visuals->chams->streamProof);

	const auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(info.m_entIndex));
	std::string_view name = info.m_model->m_name;

	constexpr std::string_view prefix = "models/weapons/v_";
	if (name.starts_with(prefix))
	{
		name = name.substr(prefix.length());

		if (name.find("arms") != std::string::npos)
		{
			if (vars::visuals->chams->arms.enabled)
			{
				if (vars::visuals->chams->streamProof)
				{
					callOriginal();
				}

				streamProofDME.beginMaterialHook();

				manual::overrideChams(vars::visuals->chams->arms.index, vars::visuals->chams->arms.wireframe, vars::visuals->chams->arms.color());
				callOriginal();
				memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
				streamProofDME.endMaterialHook();

				return true;
			}
		}
		else if (auto cl = ent->clientClass(); ent && cl && cl->m_classID == CPredictedViewModel)
		{
			if (vars::visuals->chams->weapon.enabled)
			{
				if (vars::visuals->chams->streamProof)
				{
					callOriginal();
				}

				streamProofDME.beginMaterialHook();

				manual::overrideChams(vars::visuals->chams->weapon.index, vars::visuals->chams->weapon.wireframe, vars::visuals->chams->weapon.color());
				callOriginal();
				memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
				streamProofDME.endMaterialHook();

				return true;
			}
		}
	}

	return false;
}
#pragma endregion

#pragma region manual
void chams::manual::overrideChams(int styles, bool wireframe, const Color& color)
{
	const auto& mat = materials.at(styles);

	mat.material->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);

	if (mat.materialType == MaterialTypes::GLOW)
	{
		if (const auto envmap = mat.material->findVar("$envmaptint"))
			envmap->setValues(color);

		mat.material->setMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
	}

	mat.material->modulateAllColor(color);

	memory::interfaces::studioRender->forcedMaterialOverride(mat.material);
}

// this is messy arg, visible is for attachments only
void chams::manual::drawModel(Player_t* player, bool visible)
{
	const uint8_t instanceAlpha = 255;

	if (vars::visuals->chams->players.enabled)
		player->drawModel(renderFlags, instanceAlpha);

	if (vars::visuals->chams->attachement.enabled)
	{
		Entity_t* attachment = player->firstMoveChild();

		while (attachment)
		{
			if (attachment->shouldDraw())
			{
				auto& cfg = vars::visuals->chams->attachement;
				overrideChams(cfg.index, cfg.wireframe, visible ? cfg.color() : cfg.colorXQZ());

				attachment->drawModel(renderFlags, instanceAlpha);
			}
			attachment = attachment->nextMovePeer();
		}
	}
}

// some maybe "more engine friendly" drawing through walls with using stencils rather than doing it raw
void chams::manual::passChams(Player_t* player, CfgCham& cham)
{
	DME::shouldDisable = true;

	if (cham.ignorez)
	{
		auto ctx = memory::interfaces::matSys->getRenderContext();
		if (!ctx)
			return;

		// don't clear any, we just want to mess with stencils
		ctx->clearBuffers(false, false, true);

		ShaderStencilState_t stencilVisible;
		stencilVisible.m_bEnable = true;
		stencilVisible.m_nWriteMask = 0xFF;
		stencilVisible.m_nReferenceValue = 1;
		stencilVisible.m_nTestMask = 0x0;
		stencilVisible.m_CompareFunc = SHADER_STENCILFUNC_ALWAYS;
		stencilVisible.m_PassOp = SHADER_STENCILOP_SET_TO_REFERENCE;
		stencilVisible.m_FailOp = SHADER_STENCILOP_KEEP;
		stencilVisible.m_ZFailOp = SHADER_STENCILOP_KEEP;

		ctx->setStencilState(stencilVisible);

		overrideChams(cham.index, cham.wireframe, cham.color());
		drawModel(player, true);

		ShaderStencilState_t stencilIgnorez;
		stencilIgnorez.m_bEnable = true;
		stencilIgnorez.m_nWriteMask = 0x0;
		stencilIgnorez.m_nReferenceValue = 0;
		stencilIgnorez.m_nTestMask = 0xFF;
		stencilIgnorez.m_CompareFunc = SHADER_STENCILFUNC_EQUAL;
		stencilIgnorez.m_PassOp = SHADER_STENCILOP_KEEP;
		stencilIgnorez.m_FailOp = SHADER_STENCILOP_KEEP;
		stencilIgnorez.m_ZFailOp = SHADER_STENCILOP_KEEP;

		ctx->setStencilState(stencilIgnorez);

		overrideChams(cham.index, cham.wireframe, cham.colorXQZ());
		ctx->depthRange(0.0f, 0.01f); // 0.01f is not magic, it comes as lowest possible for pixel visibility system
		drawModel(player, false);
		ctx->depthRange(0.0f, 1.0f);

		ctx->disableStencil();

		memory::interfaces::studioRender->forcedMaterialOverride(nullptr);

		return;
	}

	// otherwise, let the engine do all

	overrideChams(cham.index, cham.wireframe, cham.color());
	drawModel(player, true);
	memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
}

#include <SDK/IVRenderView.hpp>
#include <SDK/CGlobalVars.hpp>
#include <cheats/features/backtrack/backtrack.hpp>

// saves a lot, many drawmodel calls may end up tragic for performance
#define OPTIMIZE_BT_CHAMS true

void chams::manual::drawBacktrack(Player_t* player)
{
	if (!vars::visuals->chams->backtrackEnabled)
		return;

	auto& records = backtrack::records.at(player->getIndex());
	if (records.empty())
		return;
	
#if OPTIMIZE_BT_CHAMS == true
	constexpr size_t backtrackSkips = 3;
#else
	constexpr size_t backtrackSkips = 1;
#endif
	constexpr uint8_t instanceAlpha = 255;
	const Vec3& backupOrigin = player->absOrigin();
	const Vec3& backupAngles = player->absAngles();

	if (backupOrigin == records.back().origin)
		return;

	switch (vars::visuals->chams->backtrackMode)
	{
	case E2T(BTChamsType::STABLE):
	{
		for (size_t i = 1; i < records.size(); i += backtrackSkips)
		{
			if (!backtrack::isValid(records.at(i).simtime))
				break;

			overrideChams(vars::visuals->chams->backtrackIndex, false, vars::visuals->chams->backtrackColor());

			const auto backupMemory = player->m_CachedBoneData().m_memory;

			CUtlMemory<Matrix3x4> memoryBacktrack{ };
			memoryBacktrack.m_pMemory = records.at(i).matrices.data();		

			player->m_CachedBoneData().m_memory = memoryBacktrack;

			player->drawModel(renderFlags, instanceAlpha);

			player->m_CachedBoneData().m_memory = backupMemory;

			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}

		break;
	}
	case E2T(BTChamsType::LAST_TICK):
	{
		if (backtrack::isValid(records.front().simtime))
		{
			overrideChams(vars::visuals->chams->backtrackIndex, false, vars::visuals->chams->backtrackColor());

			const auto backupMemory = player->m_CachedBoneData().m_memory;

			CUtlMemory<Matrix3x4> memoryBacktrack{ };
			memoryBacktrack.m_pMemory = records.back().matrices.data();

			player->m_CachedBoneData().m_memory = memoryBacktrack;

			player->drawModel(renderFlags, instanceAlpha);

			player->m_CachedBoneData().m_memory = backupMemory;

			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}

		break;
	}
	case E2T(BTChamsType::COLOR_CHANGE):
	{
		Color fromCfg = vars::visuals->chams->backtrackColor();

		for (size_t i = 1; i < records.size(); i += backtrackSkips)
		{
			if (!backtrack::isValid(records.at(i).simtime))
				break;

			Color color{ fromCfg.r() - (i * (1.0f / records.size())),
				i * (fromCfg.g() / records.size()), fromCfg.b(), fromCfg.a() };

			overrideChams(vars::visuals->chams->backtrackIndex, false, color);

			const auto backupMemory = player->m_CachedBoneData().m_memory;

			CUtlMemory<Matrix3x4> memoryBacktrack{ };
			memoryBacktrack.m_pMemory = records.at(i).matrices.data();

			player->m_CachedBoneData().m_memory = memoryBacktrack;

			player->drawModel(renderFlags, instanceAlpha);

			player->m_CachedBoneData().m_memory = backupMemory;

			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}

		break;
	}
	}
}

void chams::manual::run()
{
	DME::shouldDisable = false;

	if (!game::isAvailable())
		return;

	if (vars::visuals->chams->players.enabled || vars::visuals->chams->attachement.enabled || vars::visuals->chams->backtrackEnabled)
	{
		for (auto [ent, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
		{
			const auto player = ent->cast<Player_t*>();
			if (!player->isOtherTeam(game::localPlayer()))
				continue;

			drawBacktrack(player);
			passChams(player, vars::visuals->chams->players);
		}
	}
}
#pragma endregion