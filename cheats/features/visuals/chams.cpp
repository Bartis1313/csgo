#include "chams.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../backtrack/backtrack.hpp"

// this code someday needs proper rewrite, it's a mess


inline constexpr void CALL(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	hooks::drawModel::original(interfaces::modelRender, ctx, state, info, matrix);
}

void chams::overrideChams(bool ignore, bool wireframe, Color color)
{
	static auto mat = interfaces::matSys->findMaterial(XOR("debug/debugambientcube"), XOR(TEXTURE_GROUP_MODEL));
	mat->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);
	mat->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	mat->alphaModulate(color.a());
	mat->colorModulate(color.r(), color.g(), color.b());
	interfaces::modelRender->overrideMaterial(mat);
}

void chams::drawChams(Player_t* ent, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	switch (config.get<int>(vars.iChams))
	{
	case E2T(ChamsID::STATIC):
		overrideChams(false, false, config.get<Color>(vars.cChams));
		CALL(ctx, state, info, matrix);
		break;
	case E2T(ChamsID::XYZ):
		overrideChams(true, false, config.get<Color>(vars.cChamsXYZ));
		CALL(ctx, state, info, matrix);
		overrideChams(false, false, config.get<Color>(vars.cChams));
		CALL(ctx, state, info, matrix);
		break;
	default:
		break;
	}
}

void chams::drawBacktrackChams(Player_t* ent, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info)
{
	switch (config.get<int>(vars.iBacktrackChams))
	{
	case E2T(BTChamsID::STABLE):
	{
		auto record = !backtrack::records[info.m_entIndex].empty() ? &backtrack::records[info.m_entIndex] : nullptr;
		if (record)
		{
			int size = record->size();
			if (size > 0)
			{
				for (int i = 0; i < size; i++)
				{
					if (backtrack::isValid(record->at(i).simTime))
					{
						overrideChams(false, false, Color(255 - (i * (255 / record->size())), i * (255 / record->size()), 255, 30));
						CALL(ctx, state, info, record->at(i).matrix);
					}
				}
			}
		}
		break;
	}
	case E2T(BTChamsID::LAST_TICK):
	{
		auto record = !backtrack::records[info.m_entIndex].empty() ? &backtrack::records[info.m_entIndex] : nullptr;
		if (record)
		{
			if (backtrack::isValid(record->front().simTime))
			{
				overrideChams(false, false, Colors::Grey);
				CALL(ctx, state, info, record->back().matrix);
			}
		}
		break;
	}
	case E2T(BTChamsID::RAINBOW):
	{
		auto record = !backtrack::records[info.m_entIndex].empty() ? &backtrack::records[info.m_entIndex] : nullptr;
		if (record)
		{
			int size = record->size();
			if (size > 0)
			{
				for (int i = 0; i < size; i++)
				{
					if (backtrack::isValid(record->at(i).simTime))
					{
						Color color = Color::rainbowColor(interfaces::globalVars->m_realtime, 5.0f);
						overrideChams(false, false, color);
						CALL(ctx, state, info, record->at(i).matrix);
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}
}

void chams::drawModel(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	if (!config.get<int>(vars.iHandChams) && !config.get<int>(vars.iWeaponChams))
		return;

	std::string name = info.m_model->m_name;

	if (name.starts_with(XOR("models/weapons/v_")))
	{
		if (config.get<int>(vars.iHandChams))
		{
			// 17 skips the ptr to move 17 indexes, so models/weapons/v_ is skipped
			if (strstr(name.c_str() + 17, XOR("arms")))
			{
				static auto material = interfaces::matSys->findMaterial(name.c_str(), XOR(TEXTURE_GROUP_MODEL));
				overrideChams(false, false, Color(0, 180, 250, 255));
				if (vars.iHandChams == E2T(HandTypes::NO_HANDS))
				{
					material->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					interfaces::modelRender->overrideMaterial(material);
				}
				CALL(ctx, state, info, matrix);
			}
		}

		if (config.get<int>(vars.iWeaponChams))
		{
			if (!strstr(name.c_str() + 17, XOR("fists")) &&
				!strstr(name.c_str() + 17, XOR("tablet")) &&
				!strstr(name.c_str() + 17, XOR("arms")) &&
				!game::localPlayer->m_bIsScoped())
			{
				// first call the engine, so in this case weapon model is visible with effects, nice for wireframe

				static auto material = interfaces::matSys->findMaterial(name.c_str(), XOR(TEXTURE_GROUP_MODEL));
				overrideChams(false, false, Colors::Palevioletred); // - Trophy
				if (vars.iWeaponChams == E2T(HandTypes::NO_WEAPON))
				{
					material->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					interfaces::modelRender->overrideMaterial(material);
				}
				CALL(ctx, state, info, matrix);
			}
		}
	}
}

void chams::run(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	if (!config.get<bool>(vars.bChams))
		return;

	if (!game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(info.m_entIndex));

	if (!ent)
		return;

	if (!ent->isAlive())
		return;

	if (ent->isDormant())
		return;

	if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
		return;

	if (ent->isPlayer())
	{
		drawBacktrackChams(ent, ctx, state, info);
		drawChams(ent, ctx, state, info, matrix);
	}
}
