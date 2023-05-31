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

#include <cheats/hooks/drawModelExecute.hpp>
#include <cheats/helper/initable.hpp>

namespace
{
	struct ChamsHandler : hooks::DrawModelExecute
	{
		ChamsHandler()
		{
			this->registerInit(chams::initMaterials);
			this->registerRun(chams::run);
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

namespace chams
{
	void callOiriginal(const std::optional<Matrix3x4*>& data = std::nullopt);
	void overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force = true, bool call = true);
	void drawBackTrack(Player_t* ent);

	namespace stored
	{
		void* result;
		DrawModelState_t state;
		ModelRenderInfo_t info;
		Matrix3x4* matrix;
	}
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

	//streamProof.init();
}

void chams::overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force, bool call)
{
	const auto& mat = materials.at(styles);

	mat.material->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	mat.material->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);

	if (mat.materialType == MaterialTypes::GLOW)
	{
		if (const auto envmap = mat.material->findVar("$envmaptint"))
			envmap->setValues(color);

		mat.material->setMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
	}

	mat.material->modulateAllColor(color);

	if (force)
		memory::interfaces::studioRender->forcedMaterialOverride(mat.material);
	if (call)
		callOiriginal();
}

#include <cheats/hooks/hooks.hpp>

void chams::callOiriginal(const std::optional<Matrix3x4*>& data)
{
	auto matrix = stored::matrix;

	if (data.has_value())
		matrix = data.value();

	hooks::DrawModelExecute::getOriginal()(memory::interfaces::modelRender(), stored::result, stored::state, stored::info, matrix);
}

#include <SDK/IMatRenderContext.hpp>

void chams::run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	runningMyChams = false;

	stored::result = result;
	stored::state = state;
	stored::info = info;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	const auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(info.m_entIndex));
	drawBackTrack(ent);

	stored::matrix = matrix;

	if (ent && ent->isPlayer() && ent->isAlive() && ent->isOtherTeam(game::localPlayer()))
	{
		if (!vars::visuals->chams->players)
			return;

		if (vars::visuals->chams->enabledXQZPlayers)
		{
			overrideChams(vars::visuals->chams->indexPlayers, true, false, vars::visuals->chams->colorXQZPlayers());
		}

		overrideChams(vars::visuals->chams->indexPlayers, false, false, vars::visuals->chams->colorPlayers(), true, false);

		runningMyChams = true;

		return;

	}
	if (std::string_view name = info.m_model->m_name; name.starts_with("models/weapons/v_"))
	{
		name = name.substr(17); // skip useless characters

		if (name.find("arms") != std::string::npos)
		{
			if (!vars::visuals->chams->enabledArms)
				return;

			auto mat = memory::interfaces::matSys->findMaterial(name.data(), TEXTURE_GROUP_MODEL);
			if (!mat)
				return;

			if (vars::visuals->chams->armsHide)
			{
				mat->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				memory::interfaces::studioRender->forcedMaterialOverride(mat);

				runningMyChams = true;

				return;
			}

			overrideChams(vars::visuals->chams->indexArms, false, false, vars::visuals->chams->colorArms(), true, false);

			runningMyChams = true;

			return;
		}
		else if (name.find("fists") == std::string::npos &&
			name.find("tablet") == std::string::npos &&
			name.find("arms") == std::string::npos &&
			!game::localPlayer->m_bIsScoped())
		{
			if (!vars::visuals->chams->enabledWeapons)
				return;

			auto mat = memory::interfaces::matSys->findMaterial(name.data(), TEXTURE_GROUP_MODEL);
			if (!mat)
				return;

			if (vars::visuals->chams->weaponHide)
			{
				mat->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				memory::interfaces::studioRender->forcedMaterialOverride(mat);

				runningMyChams = true;

				return;
			}

			overrideChams(vars::visuals->chams->indexWeapons, false, false, vars::visuals->chams->colorWeapons(), true, false);

			runningMyChams = true;

			return;
		}
	}
}

#include "../../backtrack/backtrack.hpp"
#include <SDK/CGlobalVars.hpp>

void chams::drawBackTrack(Player_t* ent)
{
	if (!vars::visuals->chams->enabledBacktrack)
		return;

	if (!ent)
		return;

	if (!ent->isAlive())
		return;

	if (!ent->isOtherTeam(game::localPlayer())) // backtrack works only for enemies here anyway
		return;

	auto& record = backtrack::records.at(stored::info.m_entIndex);
	if (record.empty())
		return;

	switch (vars::visuals->chams->indexBacktrack)
	{
	case E2T(BTChamsType::STABLE):
	{
		for (size_t i = 0; i < record.size(); i++)
		{
			if (!backtrack::isValid(record.at(i).simtime))
				break;

			overrideChams(vars::visuals->chams->modeBacktrack, false, false, vars::visuals->chams->colorBacktrack(), true, false);
			callOiriginal(record.at(i).matrices.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}

		runningMyChams = true;

		break;
	}
	case E2T(BTChamsType::LAST_TICK):
	{
		if (backtrack::isValid(record.front().simtime))
		{
			overrideChams(vars::visuals->chams->modeBacktrack, false, false, vars::visuals->chams->colorBacktrack(), true, false);
			callOiriginal(record.back().matrices.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsType::RAINBOW):
	{
		for (size_t i = 0; i < record.size(); i++)
		{
			if (!backtrack::isValid(record.at(i).simtime))
				break;

			overrideChams(vars::visuals->chams->modeBacktrack, false, false,
				Color::rainbowColor(memory::interfaces::globalVars->m_curtime + (i / 3.0f), vars::visuals->chams->rainbowBacktrackSpeed), true, false);
			callOiriginal(record.at(i).matrices.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}

		runningMyChams = true;

		break;
	}
	case E2T(BTChamsType::COLOR_CHANGE):
	{
		Color fromCfg = vars::visuals->chams->colorBacktrack();

		for (size_t i = 0; i < record.size(); i++)
		{
			if (!backtrack::isValid(record.at(i).simtime))
				break;

			Color color{ fromCfg.r() - (i * (1.0f / record.size())),
				i * (fromCfg.g() / record.size()), fromCfg.b(), fromCfg.a() };

			overrideChams(vars::visuals->chams->modeBacktrack, false, false, color, true, false);
			callOiriginal(record.at(i).matrices.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		runningMyChams = true;

		break;
	}
	default:
		break;
	}
}
