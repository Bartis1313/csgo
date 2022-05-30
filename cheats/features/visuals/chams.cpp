#include "chams.hpp"

#include <format>

#include "../../../SDK/material.hpp"
#include "../../../SDK/IMaterialSystem.hpp"
#include "../../../SDK/KeyValuesSys.hpp"
#include "../../../SDK/KeyValues.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/IVModelRender.hpp"
#include "../../../SDK/IVModelInfo.hpp"
#include "../../../SDK/IVStudioRender.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../utilities/utilities.hpp"
#include "../../../config/vars.hpp"
#include "../../game.hpp"

Chams::~Chams()
{
	for (const auto el : m_keyBuf)
		delete el;
}

void Chams::init()
{
	static auto bOnce = [this]()
	{
		m_flat = interfaces::matSys->createMaterial(XOR("Flat"), KeyValues::fromString(XOR("UnlitGeneric")));
		m_generic = interfaces::matSys->createMaterial(XOR("Generic"), KeyValues::fromString(XOR("VertexLitGeneric")));
		m_glow =
		{
			interfaces::matSys->createMaterial(XOR("Glow"), KeyValues::fromString(XOR("VertexLitGeneric"),
			XOR("$additive 1 $envmap models/effects/cube_white $envmapfresnel 1"))), true
		};
		m_metalic = interfaces::matSys->createMaterial(XOR("Metalic"), KeyValues::fromString(XOR("VertexLitGeneric"),
			XOR("$basetexture white $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1")));
		m_pearlescent = interfaces::matSys->createMaterial(XOR("Pearlscent"), KeyValues::fromString(XOR("VertexLitGeneric"),
			XOR("$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1")));

		return true;
	} ();
}

Chams::Mat_t Chams::createFromBuf(const std::string& name, bool ignore, bool wireframe, const std::string& shader, const std::string& baseTexture,
	const std::string& envMap, const std::string& proxies)
{
	/*auto retBoolStr = [](bool str) -> std::string
	{
		return str ? XOR("1") : XOR("0");
	};*/

	const std::string buf = FORMAT(XOR(R"#("{0}"
	{{
		"$basetexture"		"{1}"
		"$envmap"			"{2}"
		"$envmapfresnel"	"0"
		"$model"			"1"
		"$translucent"		"0"
		"$ignorez"			"{3}"
		"$selfillum"		"1"
		"$halflambert"		"1"
		"$wireframe"		"{4}"
		"$nofog"			"1"
		"proxies"
		{{
			{5}
		}}
	}})#"
	), shader, baseTexture, envMap, /*retBoolStr(ignore)*/ ignore, /*retBoolStr(wireframe)*/ wireframe, proxies);

	auto key = new KeyValues{ buf.c_str() };
	m_keyBuf.push_back(key);
	key->loadFromBuffer(name.c_str(), buf.c_str());

	return Mat_t{ interfaces::matSys->createMaterial(name.c_str(), key) };
}

void Chams::overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force, bool call)
{
	// have to somehow get correct material
	Mat_t mat;
	switch (styles)
	{
	case E2T(ChamsID::FLAT):
		mat = m_flat;
		break;
	case E2T(ChamsID::GENERIC):
		mat = m_generic;
		break;
	case E2T(ChamsID::GLOW):
		mat = m_glow;
		break;
	case E2T(ChamsID::METALIC):
		mat = m_metalic;
		break;
	case E2T(ChamsID::PEARLSCENT):
		mat = m_pearlescent;
		break;
	default:
		break;
	}

	if (!mat.getMat())
		return;

	if (mat.getMat()->isError())
		return;

	if (mat.isGlow())
	{
		static bool found = false;
		auto matColor = mat.getMat()->findVar(XOR("$envmaptint"), &found);
		if (found)
			matColor->setValues(color);
	}

	mat.getMat()->modulateAllColor(color);
	if (mat.isGlow())
		mat.getMat()->setMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
	mat.getMat()->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	mat.getMat()->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);
	mat.getMat()->addRef();

	if (force)
		interfaces::studioRender->forcedMaterialOverride(mat.getMat());
	if (call)
		CALL(m_result, m_state, m_info, m_matrix);
}

#include "../../hooks/hooks.hpp"

void Chams::CALL(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	hooks::drawModel::original(interfaces::modelRender, result, state, info, matrix);
}

void Chams::run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	init(); // will run once

	m_result = result;
	m_state = state;
	m_info = info;
	m_matrix = matrix;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(info.m_entIndex));
	drawBackTrack(ent);

	if (ent && ent->isPlayer() && ent->isAlive() && ent->m_iTeamNum() != game::localPlayer->m_iTeamNum())
	{
		if (!config.get<bool>(vars.bChamsPlayers))
			return;

		if (config.get<bool>(vars.bChamsXQZPlayers))
		{
			overrideChams(config.get<int>(vars.iChamsPlayers), true, false, config.get<Color>(vars.cChamsXQZPlayers));
		}

		overrideChams(config.get<int>(vars.iChamsPlayers), false, false, config.get<Color>(vars.cChamsPlayers), true, false);
		return;

	}
	if (std::string_view name = info.m_model->m_name; name.starts_with(XOR("models/weapons/v_")))
	{
		name = name.substr(17); // skip useless characters

		if (name.find(XOR("arms")) != std::string::npos)
		{
			if (!config.get<bool>(vars.bChamsArms))
				return;

			auto mat = interfaces::matSys->findMaterial(name.data(), XOR(TEXTURE_GROUP_MODEL));
			if (!mat)
				return;

			if (config.get<bool>(vars.bChamsArmsDisable))
			{
				mat->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				interfaces::studioRender->forcedMaterialOverride(mat);
				return;
			}

			overrideChams(config.get<int>(vars.iChamsArms), false, false, config.get<Color>(vars.cChamsArms), true, false);
			return;
		}
		else if (name.find(XOR("fists")) == std::string::npos &&
			name.find(XOR("tablet")) == std::string::npos &&
			name.find(XOR("arms")) == std::string::npos &&
			!game::localPlayer->m_bIsScoped())
		{
			if (!config.get<bool>(vars.bChamsWeapons))
				return;

			auto mat = interfaces::matSys->findMaterial(name.data(), XOR(TEXTURE_GROUP_MODEL));
			if (!mat)
				return;

			if (config.get<bool>(vars.bChamsWeaponsDisable))
			{
				mat->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				interfaces::studioRender->forcedMaterialOverride(mat);
				return;
			}

			overrideChams(config.get<int>(vars.iChamsWeapons), false, false, config.get<Color>(vars.cChamsWeapons), true, false);
			return;
		}
	}
}

#include "../backtrack/backtrack.hpp"
#include "../../../SDK/CGlobalVars.hpp"

void Chams::drawBackTrack(Player_t* ent)
{
	if (!config.get<bool>(vars.bChamsBacktrack))
		return;

	if (!ent)
		return;

	if (!ent->isAlive())
		return;

	if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum()) // backtrack works only for enemies here anyway
		return;

	auto record = &backtrack.getAllRecords().at(m_info.m_entIndex);
	if (!record)
		return;

	if (record->empty())
		return;

	switch (config.get<int>(vars.iChamsBacktrack))
	{
	case E2T(BTChamsID::STABLE):
	{
		for (size_t i = 0; i < record->size(); i++)
		{
			if (!backtrack.isValid(record->at(i).m_simtime))
				continue;

			overrideChams(config.get<int>(vars.iChamsBacktrackMode), false, false, config.get<Color>(vars.cChamsBacktrack), true, false);
			CALL(m_result, m_state, m_info, record->at(i).m_matrix.data());
			interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsID::LAST_TICK):
	{
		if (backtrack.isValid(record->front().m_simtime))
		{
			overrideChams(config.get<int>(vars.iChamsBacktrackMode), false, false, config.get<Color>(vars.cChamsBacktrack), true, false);
			CALL(m_result, m_state, m_info, record->back().m_matrix.data());
			interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsID::RAINBOW):
	{
		for (size_t i = 0; i < record->size(); i++)
		{
			if (!backtrack.isValid(record->at(i).m_simtime))
				continue;

			overrideChams(config.get<int>(vars.iChamsBacktrackMode), false, false,
				Color::rainbowColor(interfaces::globalVars->m_curtime + (i / 3.0f), config.get<float>(vars.fChamsBacktrackRainbowSpeed)), true, false);
			CALL(m_result, m_state, m_info, record->at(i).m_matrix.data());
			interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	case E2T(BTChamsID::COLOR_CHANGE):
	{
		Color fromCfg = config.get<Color>(vars.cChamsBacktrack);
		
		for (size_t i = 0; i < record->size(); i++)
		{
			if (!backtrack.isValid(record->at(i).m_simtime))
				continue;
			
			Color color(fromCfg.r() - (i * (1.0f / record->size())),
				i * (fromCfg.g() / record->size()), fromCfg.b(), fromCfg.a());
			
			overrideChams(config.get<int>(vars.iChamsBacktrackMode), false, false, color, true, false);
			CALL(m_result, m_state, m_info, record->at(i).m_matrix.data());
			interfaces::studioRender->forcedMaterialOverride(nullptr);
		}
		break;
	}
	default:
		break;
	}
}