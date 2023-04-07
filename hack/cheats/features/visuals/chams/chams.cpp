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
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/materialInit.hpp>

#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>

std::optional<Mat_t> Chams::addMaterialByBuffer(const Mat_t& material)
{
	KeyValues* key = new KeyValues(material.data.key.c_str());
	key->fromBuffer(material.data.name.c_str(), material.data.buf.c_str());

	Mat_t matToPush = material;
	matToPush.mat = memory::interfaces::matSys->createMaterial(material.data.name.c_str(), key);

	if (!matToPush.mat->isError())
		matToPush.mat->addRefCount();
	else
	{
		console::error("Material {} error", material.data.name);
		return std::nullopt;
	}

	console::debug("Adding material {}", material.data.name);

	return matToPush;
}

std::optional<Mat_t> Chams::addMaterialByString(const Mat_t& material)
{
	Mat_t matToPush = material;
	matToPush.mat = memory::interfaces::matSys->createMaterial(material.data.name.c_str(),
		KeyValues::fromString(material.data.key.c_str(), material.data.buf.c_str()));

	if (!matToPush.mat->isError())
		matToPush.mat->addRefCount();
	else
	{
		console::error("Material {} error", material.data.name);
		return std::nullopt;
	}

	console::debug("Adding material {}", material.data.name);

	return matToPush;
}

void Chams::initMaterials()
{
	m_materials.emplace_back(addMaterialByString(Mat_t{ .data = Mat_t::Data{.name = "Flat", .key = "UnlitGeneric" } }).value());
	m_materials.emplace_back(addMaterialByString(Mat_t{ .data = Mat_t::Data{.name = "Generic", .key = "VertexLitGeneric" } }).value());
	m_materials.emplace_back(addMaterialByString(Mat_t{ .type = Mat_t::ExtraType::GLOW, .data = Mat_t::Data{.name = "Glow", .key = "VertexLitGeneric",
		.buf = "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $envmaptint 1" } }).value());
	m_materials.emplace_back(addMaterialByString(Mat_t{ .data = Mat_t::Data{.name = "Metalic", .key = "VertexLitGeneric",
		.buf = "$basetexture white $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1"} }).value());
	m_materials.emplace_back(addMaterialByString(Mat_t{ .data = Mat_t::Data{.name = "Pearlescent", .key = "VertexLitGeneric",
		.buf = "$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1" } }).value());

	g_MaterialEditor->initEditor();
}

void Chams::overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force, bool call)
{
	Mat_t mat = m_materials.at(styles);
	
	if (!mat)
		return;

	if (mat->isError())
		return;

	mat->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	mat->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);

	if (mat.type == Mat_t::ExtraType::GLOW)
	{
		if (const auto envmap = mat->findVar("$envmaptint"))
			envmap->setValues(color);

		mat->setMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
	}

	mat->modulateAllColor(color);

	if (force)
		memory::interfaces::studioRender->forcedMaterialOverride(mat.mat);
	if (call)
		CALL(m_matrix);
}

#include <cheats/hooks/hooks.hpp>

void Chams::CALL(Matrix3x4* matrix)
{
	hooks::drawModel::original(memory::interfaces::modelRender(), m_result, m_state, m_info, matrix);
}

void Chams::run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	INIT_MATERIALS_ONCE(initMaterials);

	m_result = result;
	m_state = state;
	m_info = info;
	m_matrix = matrix;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(info.m_entIndex));
	drawBackTrack(ent);

	if (ent && ent->isPlayer() && ent->isAlive() && ent->isOtherTeam(game::localPlayer()))
	{
		if (!vars::visuals->chams->players)
			return;

		if (vars::visuals->chams->enabledXQZPlayers)
		{
			overrideChams(vars::visuals->chams->indexPlayers, true, false, vars::visuals->chams->colorXQZPlayers());
		}

		overrideChams(vars::visuals->chams->indexPlayers, false, false, vars::visuals->chams->colorPlayers(), true, false);
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
				return;
			}

			overrideChams(vars::visuals->chams->indexArms, false, false, vars::visuals->chams->colorArms(), true, false);
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
				return;
			}

			overrideChams(vars::visuals->chams->indexWeapons, false, false, vars::visuals->chams->colorWeapons(), true, false);
			return;
		}
	}
}

#include "../../backtrack/backtrack.hpp"
#include <SDK/CGlobalVars.hpp>

void Chams::drawBackTrack(Player_t* ent)
{
	if (!vars::visuals->chams->enabledBacktrack)
		return;

	if (!ent)
		return;

	if (!ent->isAlive())
		return;

	if (!ent->isOtherTeam(game::localPlayer())) // backtrack works only for enemies here anyway
		return;

	auto record = &g_Backtrack->getAllRecords().at(m_info.m_entIndex);
	if (!record)
		return;

	if (record->empty())
		return;

	switch (vars::visuals->chams->indexBacktrack)
	{
	case E2T(BTChamsType::STABLE):
	{
		for (size_t i = 0; i < record->size(); i++)
		{
			if (!g_Backtrack->isValid(record->at(i).m_simtime))
				break;

			overrideChams(vars::visuals->chams->modeBacktrack, false, false, vars::visuals->chams->colorBacktrack(), true, false);
			CALL(record->at(i).m_matrix.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsType::LAST_TICK):
	{
		if (g_Backtrack->isValid(record->front().m_simtime))
		{
			overrideChams(vars::visuals->chams->modeBacktrack, false, false, vars::visuals->chams->colorBacktrack(), true, false);
			CALL(record->back().m_matrix.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsType::RAINBOW):
	{
		for (size_t i = 0; i < record->size(); i++)
		{
			if (!g_Backtrack->isValid(record->at(i).m_simtime))
				break;

			overrideChams(vars::visuals->chams->modeBacktrack, false, false,
				Color::rainbowColor(memory::interfaces::globalVars->m_curtime + (i / 3.0f), vars::visuals->chams->rainbowBacktrackSpeed), true, false);
			CALL(record->at(i).m_matrix.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsType::COLOR_CHANGE):
	{
		Color fromCfg = vars::visuals->chams->colorBacktrack();

		for (size_t i = 0; i < record->size(); i++)
		{
			if (!g_Backtrack->isValid(record->at(i).m_simtime))
				break;

			Color color(fromCfg.r() - (i * (1.0f / record->size())),
				i * (fromCfg.g() / record->size()), fromCfg.b(), fromCfg.a());

			overrideChams(vars::visuals->chams->modeBacktrack, false, false, color, true, false);
			CALL(record->at(i).m_matrix.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	default:
		break;
	}
}
