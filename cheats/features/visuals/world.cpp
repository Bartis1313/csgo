#include "world.hpp"

#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/ClientClass.hpp"
#include "../../../SDK/IMaterialSystem.hpp"
#include "../../../SDK/IVModelInfo.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/IViewRenderBeams.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../config/vars.hpp"
#include "../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"
#include "../../globals.hpp"

void World::drawMisc()
{
	const auto maxIndex = interfaces::entList->getHighestIndex();
	for (int i = 1; i <= maxIndex; i++)
	{
		auto entity = reinterpret_cast<Entity_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (entity->isDormant())
			continue;

		if (entity->isPlayer())
			continue;

		const auto cl = entity->clientClass();

		if (!cl)
			continue;

		if (entity)
		{
			switch (cl->m_classID)
			{
			case CInferno:
				drawMolotovPoints(entity);
				break;
			case CC4:
				drawBombDropped(entity);
				break;
			case CPlantedC4:
				drawBomb(entity);
				break;
			default:
				drawProjectiles(entity);
				break;
			}
		}
	}
}

void World::drawBombDropped(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawDroppedC4))
		return;

	auto ownerHandle = ent->m_hOwnerEntity();
	auto ownerEntity = interfaces::entList->getClientFromHandle(ownerHandle);

	if (!ownerEntity)
	{
		if (Vector screen; imRender.worldToScreen(ent->absOrigin(), screen))
			imRender.text(screen.x, screen.y, ImFonts::tahoma, XOR("Dropped bomb"), false, config.get<Color>(vars.cDrawDroppedC4));
	}
}

void World::drawBomb(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawBomb))
		return;

	const static auto tickbomb = interfaces::cvar->findVar(XOR("mp_c4timer"))->getFloat();
	const auto bombent = reinterpret_cast<Bomb_t*>(ent);
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;

	if (bombtime < 0.0f)
		return;

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/389530-bomb-damage-indicator.html

	constexpr float sigma = (500.0f * 3.5f) / 3.0f;

	const float hypDist = (ent->getEyePos() - game::localPlayer->getEyePos()).length();
	const float dmg = utilities::scaleDamageArmor((500.f * (std::exp(-hypDist * hypDist / (2.0f * sigma * sigma)))), game::localPlayer->m_ArmorValue());

	const bool isSafe = dmg < game::localPlayer->m_iHealth();

	if (Box box; utilities::getBox(ent, box))
	{
		imRender.text(box.x, box.y, ImFonts::tahoma, XOR("Planted Bomb"), false, config.get<Color>(vars.cDrawBomb));
	}

	float r = (255.0f - bombtime / tickbomb * 255.0f);
	float g = (bombtime / tickbomb * 255.0f);

	imRender.text(5, 800, ImFonts::tahoma, isSafe ? std::format(XOR("TIME {:.2f} DMG {:.2f} SAFE"), bombtime, dmg) : std::format(XOR("TIME {:.2f} DMG {:.2f} YOU DIE"), bombtime, dmg), false,
		Color{ static_cast<int>(r), static_cast<int>(g), 0, 200 });
}


static bool isNade(const int classid)
{
	if (classid == CBaseCSGrenadeProjectile
		|| classid == CDecoyProjectile
		|| classid == CMolotovProjectile)
		return true;
	return false;
}

void World::drawProjectiles(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawProjectiles))
		return;

	auto cl = ent->clientClass();

	if (!cl)
		return;

	auto model = ent->getModel();

	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);

	if (!studio)
		return;

	auto projectileName = static_cast<std::string>(studio->m_name);

	if (projectileName.find(XOR("thrown")) != std::string::npos
		|| isNade(cl->m_classID))
	{
		Vector nadeOrigin = ent->absOrigin();
		Vector nadePos = {};

		if (!imRender.worldToScreen(nadeOrigin, nadePos))
			return;

		std::string nadeName = "";
		Color colorNade;

		if (projectileName.find(XOR("flashbang")) != std::string::npos)
		{
			nadeName = XOR("FLASHBANG");
			colorNade = config.get<Color>(vars.cFlashBang);
		}
		else if (projectileName.find(XOR("ggrenade")) != std::string::npos)
		{
			nadeName = XOR("GRENADE");
			colorNade = config.get<Color>(vars.cGranede);
		}
		else if (projectileName.find(XOR("molotov")) != std::string::npos)
		{
			nadeName = XOR("MOLOTOV");
			colorNade = config.get<Color>(vars.cMolotov);
		}
		else if (projectileName.find(XOR("incendiary")) != std::string::npos)
		{
			nadeName = XOR("FIRE INC");
			colorNade = config.get<Color>(vars.cIncediary);
		}
		else if (projectileName.find(XOR("smokegrenade")) != std::string::npos)
		{
			nadeName = XOR("SMOKE");
			colorNade = config.get<Color>(vars.cSmoke);
		}
		else if (projectileName.find(XOR("decoy")) != std::string::npos)
		{
			nadeName = XOR("DECOY");
			colorNade = config.get<Color>(vars.cDecoy);
		}
		else
			return;

		if (Box box; utilities::getBox(ent, box))
		{
			imRender.text(box.x + box.w / 2, box.y + box.h + 2, ImFonts::tahoma, nadeName, false, colorNade);
		}
	}
	else if (projectileName.find(XOR("dropped")) != std::string::npos)
	{
		const auto drop = reinterpret_cast<Weapon_t*>(ent);

		if (Box box; utilities::getBox(ent, box))
		{
			imRender.text(box.x + box.w / 2, box.y + box.h + 2, ImFonts::tahoma, drop->getWpnName(), false, config.get<Color>(vars.cDropped));
		}
	}
}

static void loadSkyBoxFunction(const char* name)
{
	using fn = void(__fastcall*)(const char*);
	static const auto forceSky = reinterpret_cast<fn>(utilities::patternScan(ENGINE_DLL, LOAD_SKY));
	forceSky(name);
}

#include "../../menu/GUI-ImGui/selections.hpp"

void World::skyboxLoad(int stage, bool isShutdown)
{
	if (!interfaces::engine->isInGame())
		return;

	// do not run when stage frames are not reached
	if (stage != FRAME_RENDER_START)
		return;

	if (int index = config.get<int>(vars.iSkyBox); index != 0 && !isShutdown) // is not none and there is no shutdown
		loadSkyBoxFunction(selections::skyboxes.at(index));
	else
	{
		static bool bOnce = [this]()
		{
			m_oldSky = interfaces::cvar->findVar(XOR("sv_skyname"));
			return true;
		} ();
		// restore the sky
		loadSkyBoxFunction(m_oldSky->m_string);
	}
}

void World::removeSky(bool isShutdown)
{
	const static auto r_3dsky = interfaces::cvar->findVar(XOR("r_3dsky"));
	r_3dsky->setValue(!config.get<bool>(vars.bRemoveSky));
	if (isShutdown) // default val reset
		r_3dsky->setValue(true);
}

//#include <map>

void World::modulateWorld(void* thisptr, float* r, float* g, float* b, bool isShutdown) // shutdown not needed, I don't know how to edit alpha properly
{
	if (!config.get<bool>(vars.bModulateColor))
		return;

	// to use this, you need to edit the hook and check for the valid address. The isusingdebugprops one. But forcing return is fairly easier
	//const static auto r_drawspecificstaticprop = interfaces::cvar->findVar("r_drawspecificstaticprop");
	//r_drawspecificstaticprop->setValue(!config.get<bool>(vars.bFixProps));

	/*std::map<const char*, Color> materialNames =
	{
		{ "World", config.get<Color>(vars.cWorldTexture) },
		{ "StaticProp", config.get<Color>(vars.cWorldProp) },
		{ "SkyBox", config.get<Color>(vars.cSkyBox) },
	};*/

	auto editColor = [=](const Color& color)
	{
		*r = color.r();
		*g = color.g();
		*b = color.b();
	};

	/*for (auto handle = interfaces::matSys->firstMaterial(); handle != interfaces::matSys->invalidMaterialFromHandle(); handle = interfaces::matSys->nextMaterial(handle))
	{*/
	auto material = reinterpret_cast<IMaterial*>(thisptr);

	if (!material)
		return;

	if (material->isError())
		return;

	auto matName = material->getTextureGroupName();

	/*for (const auto& [name, color] : materialNames)
	{
		if (auto check = matName.starts_with(name); check && !isShutdown)
			editColor(color);
		else if (check && isShutdown)
			editColor(Colors::White);
	}*/

	bool isGoodMat = false;

	// using strstr will just be useless, don't do this. We know expected strings
	// you can cast matName as string and use .compare or ==. Performance should be the same

	if (!strcmp(matName, XOR("World textures")))
	{
		editColor(config.get<Color>(vars.cWorldTexture));
		isGoodMat = true;
	}

	if (!strcmp(matName, XOR("StaticProp textures")))
	{
		editColor(config.get<Color>(vars.cWorldProp));
		isGoodMat = true;
	}

	if (!strcmp(matName, XOR("SkyBox textures")))
	{
		editColor(config.get<Color>(vars.cSkyBox));
		isGoodMat = true;
	}

	//constexpr int shaderAlpha = 5;

	//// this might be bugged, remove if needed
	//if (auto shader = material->getShaderParams()[shaderAlpha]; isGoodMat)
	//{
	//	!isShutdown
	//		? shader->setValue(config.get<float>(vars.fShaderParam) / 100.0f)
	//		: shader->setValue(1.0f); // default val reset
	//}
	//}
}

// TODO: add points, as drawing one circle is not accurate
void World::drawMolotovPoints(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawmolotovRange))
		return;

	auto molotov = reinterpret_cast<Inferno_t*>(ent);
	if (!molotov)
		return;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/cstrike15/Effects/inferno.cpp
	// here you can see ratios and everything

	//Vector min = {}, max = {};
	//ent->getRenderBounds(min, max);
	//imRender.drawCircle3DFilled(molotov->absOrigin(), 0.5f * Vector(max - min).length2D(), 32, config.get<Color>(vars.cMolotovRange));

	//imRender.text(screen.x, screen.y, ImFonts::tahoma, XOR("Molotov"), false, config.get<Color>(vars.cMolotovRangeText));

	const auto& origin = molotov->absOrigin();
	constexpr int molotovRadius = 60; // 30 * 2

	//std::vector<ImVec2> points = {};

	for (int i = 0; i < molotov->m_fireCount(); i++)
	{
		auto pos = origin + molotov->getInfernoPos(i);
		imRender.drawCircle3DFilled(pos, molotovRadius, 32, Colors::Coral, Colors::Coral);

		/*Vector2D posw;
		if (!imRender.worldToScreen(pos, posw))
			break;

		imRender.text(posw.x, posw.y, ImFonts::tahoma, std::to_string(i), false, Colors::Cyan);*/
		// points are not on edge, this will need some graph path logic, and will be done soon
	}
}

void World::drawZeusRange()
{
	if (!config.get<bool>(vars.bDrawZeusRange))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_TASER)
	{
		const static float range = weapon->getWpnInfo()->m_range;
		const Vector abs = game::localPlayer->absOrigin() + Vector(0.0f, 0.0f, 30.0f); // small correction to get correct trace visually, will still throw false positives on stairs etc...

		imRender.drawCircle3DTraced(abs, range, 32, game::localPlayer, config.get<Color>(vars.cZeusRange), true, 2.5f);

		//imRender.drawCircle3D(game::localPlayer->absOrigin(), weapon->getWpnInfo()->m_range, 32, config.get<Color>(vars.cZeusRange), true, 2.0f);
	}
}

void World::drawMovementTrail()
{
	static Vector end;

	// check this first, special case here
	if (!game::localPlayer)
		return;

	if (!config.get<bool>(vars.bRunMovementTrail))
	{
		// prepare the point to be corrected
		end = game::localPlayer->m_vecOrigin();
		return;
	}
	Color color;
	config.get<bool>(vars.bMovementRainbow)
		? color = Color::rainbowColor(interfaces::globalVars->m_realtime, config.get<float>(vars.fMovementRainbowSpeed))
		: color = config.get<Color>(vars.cMovementTrail);
	const static auto modelIndex = interfaces::modelInfo->getModelIndex(XOR("sprites/purplelaser1.vmt"));

	const Vector start = game::localPlayer->m_vecOrigin();

	BeamInfo_t info = {};

	info.m_type = TE_BEAMPOINTS;
	info.m_modelName = XOR("sprites/purplelaser1.vmt");
	info.m_modelIndex = modelIndex;
	info.m_vecStart = start;
	info.m_vecEnd = end;
	info.m_haloIndex = -1;
	info.m_haloScale = 0.0f;
	info.m_life = config.get<float>(vars.fMovementLife);
	info.m_width = 5.0f;
	info.m_endWidth = 5.0f;
	info.m_fadeLength = 0.0f;
	info.m_amplitude = 2.0;
	info.m_brightness = 255.0f;
	info.m_red = color.rMultiplied();
	info.m_green = color.gMultiplied();
	info.m_blue = color.bMultiplied();
	info.m_speed = config.get<float>(vars.fMovementBeamSpeed);
	info.m_startFrame = 0.0f;
	info.m_frameRate = 0.0f;
	info.m_segments = 2;
	info.m_renderable = true;

	if (auto myBeam = interfaces::beams->createBeamPoints(info); myBeam)
		interfaces::beams->drawBeam(myBeam);

	// change to pos after beam is drawn, since it's static it's possible
	end = start;
}