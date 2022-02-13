#include "world.hpp"
#include "../../../utilities/utilities.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include <format>

void world::drawMisc()
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


float scaleDamageArmor(float dmg, const float armor)
{
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/183347-bomb-damage-indicator.html
	if (armor > 0)
	{
		float flNew = dmg * 0.5f;
		float flArmor = (dmg - flNew) * 0.5f;

		if (flArmor > armor)
		{
			flArmor = armor * (1.f / 0.5f);
			flNew = dmg - flArmor;
		}
		dmg = flNew;
	}

	return dmg;
}
void world::drawBomb(Entity_t* ent)
{
	const static auto tickbomb = interfaces::console->findVar(XOR("mp_c4timer"))->getFloat();
	const auto bombent = reinterpret_cast<Bomb_t*>(ent);
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;

	if (bombtime < 0.0f)
		return;

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/389530-bomb-damage-indicator.html
	const float hypDist = (ent->getEyePos() - game::localPlayer->getEyePos()).Length(), sigma = (500.0f * 3.5f) / 3.0f;
	const float dmg = scaleDamageArmor((500.f * (std::exp(-hypDist * hypDist / (2.0f * sigma * sigma)))), game::localPlayer->m_ArmorValue());

	const bool isSafe = dmg < game::localPlayer->m_iHealth();

	Box box;
	if (utilities::getBox(ent, box))
	{
		render::text(box.x, box.y, fonts::tahoma, "Planted Bomb", false, Colors::LightBlue);
	}

	uint8_t r = static_cast<uint8_t>(255.0f - bombtime / tickbomb * 255.0f);
	uint8_t g = static_cast<uint8_t>(bombtime / tickbomb * 255.0f);

	render::text(5, 800, fonts::tahoma, isSafe ? std::format("TIME {:.2f} DMG {:.2f} SAFE", bombtime, dmg) : std::format("TIME {:.2f} DMG {:.2f} YOU DIE", bombtime, dmg), false, Color{ r, g, 0, 200 });
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
	removeSky->setValue(config.get<bool>(vars.bRunNight) ? false : true);	

	static bool done = false;

	if (config.get<bool>(vars.bRunNight) && !done)
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
	else if (!config.get<bool>(vars.bRunNight) && done)
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

// TODO: add points, as drawing one circle is not accurate
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

	Vector min = {}, max = {};
	ent->getRenderBounds(min, max);
	render::drawFilledCircle3D(molotov->absOrigin(), 0.5f * Vector(max - min).Length2D(), 32, Colors::Red);

	render::text(screen.x, screen.y, fonts::tahoma, XOR("Molotov"), false, Colors::White);
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
		//const static float range = weapon->getWpnInfo()->m_range;
		//const Vector abs = game::localPlayer->absOrigin() + Vector(0.0f, 0.0f, 30.0f); // small correction to get correct trace visually, will still throw false positives on stairs etc...

		//Vector screenStart = {};
		//float step = std::numbers::pi_v<float> *2.0f / 32;
		//static Vector before = NOTHING;

		//for (float i = 0; i < (M_PI * 2.0f); i += step)
		//{
		//	Vector start(range * std::cos(i) + abs.x, range * sin(i) + abs.y, abs.z);

		//	Trace_t trace;
		//	TraceFilter filter;
		//	filter.m_skip = game::localPlayer;

		//	interfaces::trace->traceRay({ abs, start }, MASK_SHOT_BRUSHONLY, &filter, &trace);

		//	if (render::worldToScreen(trace.m_end, screenStart) && before.IsValid())
		//	{
		//		render::drawLine(screenStart.x, screenStart.y, before.x, before.y, Colors::Palevioletred);
		//		before = screenStart;
		//	}
		//}
		// I think it's useless to take ^ code, if you really want to, use something as eyepos to not get false results, but then this is only useful in 3rd person
		render::drawCircle3D(game::localPlayer->absOrigin(), weapon->getWpnInfo()->m_range, 32, Colors::Palevioletred);
	}
}