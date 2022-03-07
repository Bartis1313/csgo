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
	if (!config.get<bool>(vars.bDrawBomb))
		return;

	const static auto tickbomb = interfaces::console->findVar(XOR("mp_c4timer"))->getFloat();
	const auto bombent = reinterpret_cast<Bomb_t*>(ent);
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;

	if (bombtime < 0.0f)
		return;

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/389530-bomb-damage-indicator.html

	constexpr float sigma = (500.0f * 3.5f) / 3.0f;

	const float hypDist = (ent->getEyePos() - game::localPlayer->getEyePos()).length();
	const float dmg = scaleDamageArmor((500.f * (std::exp(-hypDist * hypDist / (2.0f * sigma * sigma)))), game::localPlayer->m_ArmorValue());

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

void world::drawProjectiles(Entity_t* ent)
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

void loadSkyBoxFunction(const char* name)
{
	using fn = void(__fastcall*)(const char*);
	static const auto forceSky = reinterpret_cast<fn>(utilities::patternScan(ENGINE_DLL, LOAD_SKY));
	forceSky(name);
}

namespace skyGlobals
{
	bool done = false;
	IConVar* sky = nullptr;
}

void implySky()
{
	if (skyGlobals::done)
		return;

	for (auto handle = interfaces::matSys->firstMaterial(); handle != interfaces::matSys->invalidMaterialFromHandle(); handle = interfaces::matSys->nextMaterial(handle))
	{
		auto material = interfaces::matSys->getMaterial(handle);

		if (!material)
			continue;

		if (material->isError())
			continue;

		float brightness = config.get<float>(vars.fBrightnessWorld) / 100.0f;

		if (strstr(material->getTextureGroupName(), XOR("World textures")))
		{
			material->colorModulate(brightness, brightness, brightness);
		}

		if (strstr(material->getTextureGroupName(), XOR("StaticProp")))
		{
			material->colorModulate(brightness, brightness, brightness);
		}

		if (strstr(material->getTextureGroupName(), XOR("SkyBox")))
		{
			material->colorModulate(config.get<Color>(vars.cSky));
		}
	}
	// force to new sky
	loadSkyBoxFunction(XOR("sky_csgo_night2"));
	skyGlobals::done = true;
}

void destroySky()
{
	if (!skyGlobals::done)
		return;

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
	loadSkyBoxFunction(skyGlobals::sky->m_name);
	skyGlobals::done = false;
}

void world::skyboxLoad(int stage)
{
	if (!interfaces::engine->isInGame())
		return;

	if (!game::localPlayer)
		return;

	// do not run when stage frames are not reached
	if (stage != FRAME_RENDER_START)
		return;

	skyGlobals::sky = interfaces::console->findVar(XOR("sv_skyname"));

	bool enabled = config.get<bool>(vars.bRunNight);
	enabled ? implySky() : destroySky();

	// remove sky, not in meaning as full color
	static const auto removeSky = interfaces::console->findVar(XOR("r_3dsky"));
	removeSky->setValue(config.get<bool>(vars.bRunNight) ? false : true);	
}

// TODO: add points, as drawing one circle is not accurate
void world::drawMolotovPoints(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawmolotovRange))
		return;

	auto molotov = reinterpret_cast<Inferno_t*>(ent);
	if (!molotov)
		return;

	Vector screen = {};
	if (!imRender.worldToScreen(molotov->absOrigin(), screen))
		return;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/cstrike15/Effects/inferno.cpp
	// here you can see ratios and everything

	Vector min = {}, max = {};
	ent->getRenderBounds(min, max);
	imRender.drawCircle3DFilled(molotov->absOrigin(), 0.5f * Vector(max - min).length2D(), 32, config.get<Color>(vars.cMolotovRange));

	imRender.text(screen.x, screen.y, ImFonts::tahoma, XOR("Molotov"), false, config.get<Color>(vars.cMolotovRangeText));
}

bool w2s(const Vector& in, Vector& out)
{
	return interfaces::debugOverlay->worldToScreen(in, out) != 1;
}

void world::drawZeusRange()
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

		//imRender.drawCircle3DTraced(abs, range, 32, game::localPlayer, Colors::Palevioletred);
		 
		imRender.drawCircle3D(game::localPlayer->absOrigin(), weapon->getWpnInfo()->m_range, 32, config.get<Color>(vars.cZeusRange), true, 2.0f);
	}
}