#include "chams.hpp"
#include "../../game.hpp"
#include "../../menu/vars.hpp"
#include "../backtrack/backtrack.hpp"

#define CALL(ctx, state, info, matrix) \
hooks::drawModel::original(interfaces::modelRender, ctx, std::cref(state), std::cref(info), matrix)

enum chamsIDs
{
	STATIC =  1,
	XYZ,
};

enum BTChamsIDs
{
	STABLE = 1,
	LAST_TICK,
	RAINBOW
};

namespace chams
{
	void ovverride(bool ignore, bool wireframe, Color color)
	{
		const auto material = interfaces::matSys->findMaterial(XOR("debug/debugambientcube"), XOR(TEXTURE_GROUP_MODEL));
		material->setMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignore);
		material->setMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
		material->alphaModulate(color.aDevided());
		material->colorModulate(color.rDevided(), color.gDevided(), color.bDevided());
		interfaces::modelRender->overrideMaterial(material);
	}

	// TODO: fix this code
	void run(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
	{
		if (!vars::iChams)
			return;

		if (!game::localPlayer)
			return;

		const auto model = info.m_model;
		if (!model)
			return;

		auto isPlayer = strstr(model->m_name, XOR("models/player")) != nullptr;

		if (isPlayer)
		{
			auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(info.m_entIndex));

			if (!ent || !ent->isAlive() || ent->isDormant())
				return;

			// TODO: if ent->m_bGunGameImmunity then draw invisible, I think the best option and easy

			if (ent->m_iTeamNum() != game::localPlayer->m_iTeamNum())
			{
				switch (vars::iBacktrackChams)
				{
				case STABLE:
				{
					auto record = &backtrack::records[info.m_entIndex];
					if (record)
					{
						int size = record->size();
						if (size > 0)
						{
							for (int i = 0; i < size; i++)
							{
								if (backtrack::isValid(record->at(i).simTime))
								{
									ovverride(true, false, Color(255 - (i * (255 / record->size())), i * (255 / record->size()), 255, 30));
									CALL(ctx, state, info, record->at(i).matrix);
								}
							}
						}
					}
					break;
				}
				case LAST_TICK:
				{
					auto record = &backtrack::records[info.m_entIndex];
					if (record)
					{
						if (backtrack::isValid(record->front().simTime))
						{
							ovverride(true, false, Colors::Grey);
							CALL(ctx, state, info, record->back().matrix);
						}
					}
					break;
				}
				case RAINBOW:
				{
					auto record = &backtrack::records[info.m_entIndex];
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
									ovverride(true, false, color.drawRainbow(interfaces::globalVars->m_frametime, 0.70f, 0.8f, 0.01f));
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
			// else draw teammates or localp
		}
	}
}