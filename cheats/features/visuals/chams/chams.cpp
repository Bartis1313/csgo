#include "chams.hpp"

#include <format>

#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/KeyValuesSys.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IVStudioRender.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
#include <config/vars.hpp>
#include <game/game.hpp>

Chams::Mat_t::Mat_t(IMaterial* mat, Chams::Mat_t::ExtraType type)
	: mat{ mat }, type{ type }
{
	if (mat)
		mat->addRef();
}

void Chams::init()
{
	m_flat = 
	{ 
		memory::interfaces::matSys->createMaterial(XOR("Flat"), KeyValues::fromString(XOR("UnlitGeneric")))
	};
	m_generic =
	{ 
		memory::interfaces::matSys->createMaterial(XOR("Generic"), KeyValues::fromString(XOR("VertexLitGeneric"))) 
	};
	m_glow =
	{ 
		memory::interfaces::matSys->createMaterial(XOR("Glow"), KeyValues::fromString(XOR("VertexLitGeneric"),
		XOR("$additive 1 $envmap models/effects/cube_white $envmapfresnel 1"))),
		Mat_t::ExtraType::GLOW
	};
	m_metalic = 
	{
		memory::interfaces::matSys->createMaterial(XOR("Metalic"), KeyValues::fromString(XOR("VertexLitGeneric"),
		XOR("$basetexture white $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1")))
	};
	m_pearlescent =
	{
		memory::interfaces::matSys->createMaterial(XOR("Pearlscent"), KeyValues::fromString(XOR("VertexLitGeneric"),
		XOR("$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1")))
	};
}

void Chams::overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force, bool call)
{
	static std::unordered_map<ChamsType, Mat_t> materials
	{
		{ ChamsType::FLAT, m_flat },
		{ ChamsType::GENERIC, m_generic },
		{ ChamsType::METALIC, m_glow },
		{ ChamsType::FLAT, m_metalic },
		{ ChamsType::PEARLSCENT, m_pearlescent },
	};

	Mat_t mat = materials.at(static_cast<ChamsType>(styles));
	
	if (!mat())
		return;

	if (mat->isError())
		return;

	if (mat.type == Mat_t::ExtraType::GLOW)
	{
		static bool found = false;
		auto matColor = mat->findVar(XOR("$envmaptint"), &found);
		if (found)
			matColor->setValues(color);
	}

	mat->modulateAllColor(color);
	if (mat())
		mat->setMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
	mat->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	mat->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);

	if (force)
		memory::interfaces::studioRender->forcedMaterialOverride(mat.mat);
	if (call)
		CALL(m_result, m_state, m_info, m_matrix);
}

#include <hooks/hooks.hpp>

void Chams::CALL(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	hooks::drawModel::original(memory::interfaces::modelRender(), result, state, info, matrix);
}

void Chams::run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
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
	if (std::string_view name = info.m_model->m_name; name.starts_with(XOR("models/weapons/v_")))
	{
		name = name.substr(17); // skip useless characters

		if (name.find(XOR("arms")) != std::string::npos)
		{
			if (!vars::visuals->chams->enabledArms)
				return;

			auto mat = memory::interfaces::matSys->findMaterial(name.data(), XOR(TEXTURE_GROUP_MODEL));
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
		else if (name.find(XOR("fists")) == std::string::npos &&
			name.find(XOR("tablet")) == std::string::npos &&
			name.find(XOR("arms")) == std::string::npos &&
			!game::localPlayer->m_bIsScoped())
		{
			if (!vars::visuals->chams->enabledWeapons)
				return;

			auto mat = memory::interfaces::matSys->findMaterial(name.data(), XOR(TEXTURE_GROUP_MODEL));
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
			CALL(m_result, m_state, m_info, record->at(i).m_matrix.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsType::LAST_TICK):
	{
		if (g_Backtrack->isValid(record->front().m_simtime))
		{
			overrideChams(vars::visuals->chams->modeBacktrack, false, false, vars::visuals->chams->colorBacktrack(), true, false);
			CALL(m_result, m_state, m_info, record->back().m_matrix.data());
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
			CALL(m_result, m_state, m_info, record->at(i).m_matrix.data());
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
			CALL(m_result, m_state, m_info, record->at(i).m_matrix.data());
			memory::interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	default:
		break;
	}
}