#include "world.hpp"
#include "../../../utilities/utilities.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include <format>

void world::drawMisc()
{
	for (int i = 1; i < interfaces::entList->getHighestIndex(); i++)
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

void world::drawBombDropped(Entity_t* ent)
{
	auto ownerHandle = ent->m_hOwnerEntity();
	auto ownerEntity = interfaces::entList->getClientFromHandle(ownerHandle);

	if (!ownerEntity)
	{
		Vector screen = {};
		if (render::worldToScreen(ent->absOrigin(), screen))
			render::text(screen.x, screen.y, fonts::tahoma, "Dropped bomb", false, Colors::White);
	}
}

void world::drawBomb(Entity_t* ent)
{
	const auto tickbomb = interfaces::console->findVar(XOR("mp_c4timer"))->getFloat();
	const auto bombent = reinterpret_cast<Bomb_t*>(ent);
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;

	if (bombtime < 0.0f)
		return;

	Box box;
	if (utilities::getBox(ent, box))
	{
		render::text(box.x, box.y, fonts::tahoma, "Planted Bomb", false, Colors::LightBlue);
		render::drawOutlineRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0, 200));
		render::drawOutlineRect(box.x, box.y, box.w, box.h, Color(120, 60, 0, 200));
		render::drawOutlineRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 200));
	}

	uint8_t r = static_cast<uint8_t>(255.0f - bombtime / tickbomb * 255.0f);
	uint8_t g = static_cast<uint8_t>(bombtime / tickbomb * 255.0f);

	render::text(5, 800, fonts::tahoma, std::format("TIME: {:.3f}", bombtime), false, Color{ r, g, 0, 200 });
}


static bool isNade(const int classid)
{
	if (classid == CBaseCSGrenadeProjectile
		|| classid == CDecoyProjectile
		|| classid == CMolotovProjectile)
		return true;
	return false;
}

void world::drawProjectiles(Entity_t* ent)
{
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

	if (projectileName.find("thrown") != std::string::npos
		|| isNade(cl->m_classID))
	{
		Vector nadeOrigin = ent->absOrigin();
		Vector nadePos = {};

		if (!render::worldToScreen(nadeOrigin, nadePos))
			return;

		std::string nadeName = "";
		Color colorNade;

		if (projectileName.find("flashbang") != std::string::npos)
		{
			nadeName = "FLASHBANG";
			colorNade = Color(130, 0, 200, 255);
		}
		else if (projectileName.find("ggrenade") != std::string::npos)
		{
			nadeName = "GRENADE";
			colorNade = Color(130, 180, 20, 255);
		}
		else if (projectileName.find("molotov") != std::string::npos)
		{
			nadeName = "MOLOTOV";
			colorNade = Color(130, 200, 120, 255);
		}
		else if (projectileName.find("incendiary") != std::string::npos)
		{
			nadeName = "FIRE INC";
			colorNade = Color(130, 200, 120, 255);
		}
		else if (projectileName.find("smokegrenade") != std::string::npos)
		{
			nadeName = "SMOKE";
			colorNade = Color(20, 70, 30, 255);
		}
		else if (projectileName.find("decoy") != std::string::npos)
		{
			nadeName = "DECOY";
			colorNade = Color(0, 30, 60, 255);
		}
		else
			return;

		Box box;

		if (utilities::getBox(ent, box))
		{
			render::text(box.x + box.w / 2, box.y + box.h + 2, fonts::tahoma, nadeName, false, colorNade);
		}
	}
	else if (projectileName.find("dropped") != std::string::npos)
	{
		const auto drop = reinterpret_cast<Weapon_t*>(ent);

		Box box;

		if (utilities::getBox(ent, box))
		{
			render::text(box.x + box.w / 2, box.y + box.h + 2, fonts::tahoma, drop->getWpnName(), false, Colors::White);
		}
	}
}

void loadSkyBoxFunction(const char* name)
{
	using fn = void(__fastcall*)(const char*);
	static const auto forceSky = reinterpret_cast<fn>(utilities::patternScan(ENGINE_DLL, LOAD_SKY));
	forceSky(name);
}

void world::skyboxLoad(int stage)
{
	if (!interfaces::engine->isInGame())
		return;

	// do not run when stage frames are not reached
	if (stage != FRAME_RENDER_START)
		return;

	static auto sky = interfaces::console->findVar(XOR("sv_skyname"));

	// remove sky, not in meaning as full color
	static const auto removeSky = interfaces::console->findVar(XOR("r_3dsky"));
	removeSky->setValue(vars::bRunNight ? false : true);	

	static bool done = false;

	if (vars::bRunNight && !done)
	{
		for (auto handle = interfaces::matSys->firstMaterial(); handle != interfaces::matSys->invalidMaterialFromHandle(); handle = interfaces::matSys->nextMaterial(handle))
		{
			auto material = interfaces::matSys->getMaterial(handle);

			if (!material)
				continue;

			if (material->isError())
				continue;

			if (strstr(material->getTextureGroupName(), XOR("World textures")))
			{
				material->colorModulate(0.2f, 0.2f, 0.2f);
			}

			if (strstr(material->getTextureGroupName(), XOR("StaticProp")))
			{
				material->colorModulate(0.2f, 0.2f, 0.2f);
			}

			if (strstr(material->getTextureGroupName(), XOR("SkyBox")))
			{
				material->colorModulate(Colors::Turquoise);
			}
		}
		// force to new sky
		loadSkyBoxFunction(XOR("sky_csgo_night2"));
		done = true;
	}
	else if (!vars::bRunNight && done)
	{
		for (auto handle = interfaces::matSys->firstMaterial(); handle != interfaces::matSys->invalidMaterialFromHandle(); handle = interfaces::matSys->nextMaterial(handle))
		{
			auto material = interfaces::matSys->getMaterial(handle);

			if (!material)
				continue;

			if (material->isError())
				continue;
	
			if (strstr(material->getTextureGroupName(), XOR("StaticProp")))
			{
				material->colorModulate(Colors::White);
			}

			if (strstr(material->getTextureGroupName(), XOR("World textures")))
			{
				material->colorModulate(Colors::White);
			}

			if (strstr(material->getTextureGroupName(), XOR("SkyBox")))
			{
				material->colorModulate(Colors::White);
			}
		}
		// restore the sky
		loadSkyBoxFunction(sky->m_name);
		done = false;
	}
}

// man I am too dumb for this, this is TODO
void world::drawMolotovPoints(Entity_t* ent)
{
	auto molotov = reinterpret_cast<Inferno_t*>(ent);
	if (!molotov)
		return;

	Vector screen = {};
	if (!render::worldToScreen(molotov->absOrigin(), screen))
		return;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/cstrike15/Effects/inferno.cpp
	// here you can see ratios and everything

	render::drawCircle3D(molotov->absOrigin() + Vector(0, 60, 0), 60, 32, Colors::Red);

	render::text(screen.x, screen.y + 60, fonts::tahoma, XOR("Molotov"), false, Colors::White);
}

void world::drawZeusRange()
{
	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_TASER)
	{
		render::drawCircle3D(game::localPlayer->absOrigin(), weapon->getWpnInfo()->m_range, 32, Colors::Palevioletred);
	}
}