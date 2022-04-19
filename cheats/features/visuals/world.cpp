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
#include "../../../SDK/IEffects.hpp"
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

		drawProjectiles(entity, cl->m_classID);

		switch (cl->m_classID)
		{
		case CC4:
			drawBombDropped(entity);
			break;
		case CInferno:
			drawMolotov(entity);
			break;
		case CPlantedC4:
			drawBomb(entity);
			break;
		case CSmokeGrenadeProjectile:
			drawSmoke(entity);
			break;
		default:
			break;
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

#include "player.hpp"

void World::drawProjectiles(Entity_t* ent, const int id)
{
	if (!game::isAvailable())
		return;

	auto model = ent->getModel();
	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	auto wpn = reinterpret_cast<Weapon_t*>(ent);
	if (!wpn) // should not ever happen
		return;

	constexpr auto goodID = [=](const int idx) // needed because some nades are special
	{
		constexpr std::array arrid =
		{
			CBaseCSGrenadeProjectile,
			CDecoyProjectile,
			CMolotovProjectile,
		};
		auto itr = std::find(arrid.cbegin(), arrid.cend(), idx);
		return itr != arrid.cend();
	};

	if (std::string_view projectileName = studio->m_name; projectileName.find(XOR("thrown")) != std::string::npos || goodID(id)
		&& config.get<bool>(vars.bDrawProjectiles))
	{
		std::pair<std::string, Color> nades;

		if (projectileName.find(XOR("flashbang")) != std::string::npos)
			nades = { XOR("FLASHBANG"), config.get<Color>(vars.cFlashBang) };
		if (projectileName.find(XOR("ggrenade")) != std::string::npos && wpn->m_nExplodeEffectTickBegin() < 1) // prevent too long time
			nades = { XOR("GRENADE"), config.get<Color>(vars.cGranede) };
		if (projectileName.find(XOR("molotov")) != std::string::npos)
			nades = { XOR("MOLOTOV"), config.get<Color>(vars.cMolotov) };
		if (projectileName.find(XOR("incendiary")) != std::string::npos)
			nades = { XOR("FIRE INC"), config.get<Color>(vars.cIncediary) };
		if (projectileName.find(XOR("smokegrenade")) != std::string::npos && !reinterpret_cast<Smoke_t*>(wpn)->m_nSmokeEffectTickBegin()) // prevent too long time
			nades = { XOR("SMOKE"), config.get<Color>(vars.cSmoke) };
		if (projectileName.find(XOR("decoy")) != std::string::npos) // this nade time is also too long, to prevent it you need to check tick time. There is no netvar for decoys
			nades = { XOR("DECOY"), config.get<Color>(vars.cDecoy) };

		if (Box box; utilities::getBox(ent, box))
			imRender.text(box.x + box.w / 2, box.y + box.h + 2, ImFonts::verdana, nades.first, true, nades.second);
	}
	else if (projectileName.find(XOR("dropped")) != std::string::npos && config.get<bool>(vars.bDrawDropped))
	{
		if (Box box; utilities::getBox(ent, box))
		{
			float fontSize = visuals.getScaledFontSize(ent, 60.0f, 11.0f, 16.0f);
			using cont = std::vector<bool>; // container
			float padding = 0.0f;

			auto getTextSize = [=](const std::string& text, ImFont* font = ImFonts::verdana)
			{
				return font->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());
			};

			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::BOX))) // startpoint - no pad
			{
				Color cfgCol = config.get<Color>(vars.cDrawDropped);

				imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Colors::Black);
				imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Colors::Black);
				imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol);
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::AMMO)) && !wpn->isNonAimable()) // no pad font logic, we just draw extra box
			{
				constexpr float startPad = 3.0f;

				imRender.drawRectFilled(box.x - 1.0f, box.y + box.h - 1.0f + startPad, box.w + 2.0f, 4.0f, Colors::Black);
				imRender.drawRectFilled(box.x, box.y + box.h + startPad,
					wpn->m_iClip1() * box.w / wpn->getWpnInfo()->m_maxClip1, 2.0f, config.get<Color>(vars.cDrawDropped));

				padding += 4.0f;
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::TEXT)))
			{
				auto name = wpn->getWpnName();
				imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::verdana,
					name, true, config.get<Color>(vars.cDropped));

				auto textSize = getTextSize(name);
				padding += textSize.y;
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::ICON)))
			{
				auto name = utilities::u8toStr(wpn->getIcon());
				imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::icon,
					name, true, config.get<Color>(vars.cDropped));

				auto textSize = getTextSize(name, ImFonts::icon);
				padding += textSize.y;
			}
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

	/*for (const auto& [name, color] : materialNames)
	{
		if (auto check = matName.starts_with(name); check && !isShutdown)
			editColor(color);
		else if (check && isShutdown)
			editColor(Colors::White);
	}*/

	bool isGoodMat = false;

	std::string_view name = material->getTextureGroupName();

	if (name == XOR("World textures"))
	{
		editColor(config.get<Color>(vars.cWorldTexture));
		isGoodMat = true;
	}

	if (name == XOR("StaticProp textures"))
	{
		editColor(config.get<Color>(vars.cWorldProp));
		isGoodMat = true;
	}

	if (name == XOR("SkyBox textures"))
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

void World::drawMolotov(Entity_t* ent)
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
	float time = molotov->spawnTime() + molotov->expireTime() - interfaces::globalVars->m_curtime;
	float scale = time / molotov->expireTime();

	const auto& origin = molotov->absOrigin();
	constexpr int molotovRadius = 60; // 30 * 2

	//std::vector<ImVec2> points = {};
	Color col = config.get<Color>(vars.cMolotovRange);

	for (int i = 0; i < molotov->m_fireCount(); i++)
	{
		auto pos = origin + molotov->getInfernoPos(i);
		imRender.drawCircle3DFilled(pos, molotovRadius, 32, col, col);

		/*Vector2D posw;
		if (!imRender.worldToScreen(pos, posw))
			break;

		imRender.text(posw.x, posw.y, ImFonts::tahoma, std::to_string(i), false, Colors::Cyan);*/
		// points are not on edge, this will need some graph path logic, and will be done soon
	}
	static float size = ImFonts::tahoma->FontSize;
	// timer
	if (Vector2D s; imRender.worldToScreen(origin, s))
	{
		imRender.drawProgressRing(s.x, s.y, 25, 32, -90.0f, scale, 5.0f, Colors::LightBlue);
		imRender.text(s.x, s.y - (size / 2.0f), ImFonts::tahoma, std::format(XOR("{:.2f}s"), time), true, Colors::White);
	}
}

void drawArc3DSmoke(const Vector& pos, const float radius, const int points, const float percent, const Color& color, const ImDrawFlags flags, const float thickness,
	ImFont* font, const std::string& text, const Color& colorText)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	Vector2D _end = {};
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f * percent); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector worldEnd = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (Vector2D start, end; imRender.worldToScreen(worldStart, start) && imRender.worldToScreen(worldEnd, end))
		{
			_end = end;
			imRender.drawLine(start, end, color);
		}
	}

	if (!_end.isZero())
		imRender.text(_end.x + 3.0f, _end.y, font, text, true, colorText);
}

void World::drawSmoke(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawSmoke))
		return;

	auto smoke = reinterpret_cast<Smoke_t*>(ent);
	if (!smoke)
		return;

	if (!smoke->m_nSmokeEffectTickBegin()) // dont do anything when smoke is thrown
		return;

	auto timeToTicks = [=](const float time)
	{
		return interfaces::globalVars->m_intervalPerTick * time;
	};

	float time = timeToTicks(smoke->m_nSmokeEffectTickBegin()) + smoke->expireTime() - interfaces::globalVars->m_curtime;
	float scale = time / smoke->expireTime();

	const auto& origin = smoke->absOrigin();
	constexpr int smokeRadius = 144;

	//imRender.drawCircle3DFilled(origin, smokeRadius, 216, col, col, true, 2.0f);
	// many points to make it smooth
	drawArc3DSmoke(origin, smokeRadius, 512, scale, config.get<Color>(vars.cDrawSmoke), false, 2.0f, ImFonts::tahoma, std::format(XOR("{:.2f}s"), time), Colors::Orange);
	
	// timer
	/*if (Vector2D s; imRender.worldToScreen(origin, s))
	{
		imRender.drawProgressRing(s.x, s.y, 25, 32, -90, scale, 5.0f, Colors::LightBlue);
		imRender.text(s.x, s.y, ImFonts::tahoma, std::format(XOR("{:.2f}s"), time), false, Colors::White);
	}*/
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
	if (!game::isAvailable())
		return;

	int type = config.get<int>(vars.iRunMovementTrail);

	if (!config.get<bool>(vars.bRunMovementTrail))
	{
		return;
	}

	if (type != E2T(MovementTrail::BEAM))
	{
		// prepare the point to be corrected
		end = game::localPlayer->m_vecOrigin();
	}

	switch (config.get<int>(vars.iRunMovementTrail))
	{
	case E2T(MovementTrail::BEAM):
	{
		if (!game::localPlayer->isMoving()) // do not add beams on not moving
			return;

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

		break;
	}
	case E2T(MovementTrail::LINE):
	{
		Color color;
		config.get<bool>(vars.bMovementRainbow)
			? color = Color::rainbowColor(interfaces::globalVars->m_realtime, config.get<float>(vars.fMovementRainbowSpeed))
			: color = config.get<Color>(vars.cMovementTrail);

		float curtime = interfaces::globalVars->m_curtime;

		if (game::localPlayer->isMoving())
			m_trails.emplace_back(Trail_t{ game::localPlayer->m_vecOrigin(), curtime + config.get<float>(vars.fMovementLife), color });

		Vector last = {};
		if (!m_trails.empty())
			last = m_trails.front().m_pos;

		for (size_t i = 0; const auto & el : m_trails)
		{
			if (el.m_expire < curtime)
			{
				m_trails.erase(m_trails.begin() + i);
				continue;
			}

			if (Vector2D start, end; !last.isZero() && imRender.worldToScreen(el.m_pos, start) && imRender.worldToScreen(last, end))
				imRender.drawLine(start, end, el.m_col, 3.0f);

			last = el.m_pos;

			i++;
		}

		break;
	}
	case E2T(MovementTrail::SPLASH):
	{
		if (game::localPlayer->isMoving())
			interfaces::effects->energySplash(game::localPlayer->m_vecOrigin(), {}, true);

		break;
	}
	default:
		break;
	}
}