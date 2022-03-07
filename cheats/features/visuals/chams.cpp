#include "chams.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../backtrack/backtrack.hpp"

inline constexpr void CALL(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	hooks::drawModel::original(interfaces::modelRender, ctx, state, info, matrix);
}

enum ChamsIDs
{
	STATIC = 1,
	XYZ,
};

enum BTChamsIDs
{
	STABLE = 1,
	LAST_TICK,
	RAINBOW,
};

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
	case STATIC:
		overrideChams(false, false, config.get<Color>(vars.cChams));
		CALL(ctx, state, info, matrix);
		break;
	case XYZ:
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
	case STABLE:
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
	case LAST_TICK:
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
	case RAINBOW:
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
						Rainbow color;
						// fine effect: (float)i / (float)size as saturation arg
						overrideChams(false, false, color.drawRainbow(interfaces::globalVars->m_frametime * 10, 0.7f, 0.8f, 0.01f)); // 10x faster cuz looks better imo - Trophy
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

enum HandTypes
{
	COLOR = 0,
	NO_HANDS,
	NO_WEAPON,
};

// TODO: fix performance
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
				if (vars.iHandChams == NO_HANDS)
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
				if (vars.iWeaponChams == NO_WEAPON)
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
