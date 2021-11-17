#include "chams.hpp"
#include "../../game.hpp"
#include "../../menu/vars.hpp"
#include "../backtrack/backtrack.hpp"

inline constexpr void CALL(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	hooks::drawModel::original(interfaces::modelRender, ctx, std::cref(state), std::cref(info), matrix);
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

void chams::ovverride(bool ignore, bool wireframe, Color color)
{
	const auto material = interfaces::matSys->findMaterial(XOR("debug/debugambientcube"), XOR(TEXTURE_GROUP_MODEL));
	material->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);
	material->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->alphaModulate(color.aDevided());
	material->colorModulate(color.rDevided(), color.gDevided(), color.bDevided());
	interfaces::modelRender->overrideMaterial(material);
}

void chams::drawChams(Player_t* ent, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	switch (vars::iChams)
	{
	case STATIC:
		ovverride(false, false, Color(255, 0, 255, 255));
		CALL(ctx, state, info, matrix);
		break;
	case XYZ:
		ovverride(true, false, Color(0, 100, 255, 255));
		CALL(ctx, state, info, matrix);
		ovverride(false, false, Color(255, 0, 255, 255));
		CALL(ctx, state, info, matrix);
		break;
	default:
		break;
	}
}

void chams::drawBacktrackChams(Player_t* ent, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info)
{
	// TODO: if ent->m_bGunGameImmunity then draw invisible, I think the best option and easy
	switch (vars::iBacktrackChams)
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
						ovverride(false, false, Color(255 - (i * (255 / record->size())), i * (255 / record->size()), 255, 30));
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
				ovverride(false, false, Colors::Grey);
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
						ovverride(false, false, color.drawRainbow(interfaces::globalVars->m_frametime, 0.7f, 0.8f, 0.01f));
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

void chams::run(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	if (!game::localPlayer)
		return;

	const auto model = info.m_model;
	if (!model)
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
		drawBacktrackChams(ent, ctx, state, std::cref(info));
		drawChams(ent, ctx, state, std::cref(info), std::cref(matrix));
	}
}