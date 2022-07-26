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
#include "../prediction/nadepred.hpp"

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

		if (cl->m_classID == CPlantedC4)
			m_bombEnt = reinterpret_cast<Bomb_t*>(entity);

		drawProjectiles(entity, cl->m_classID);

		nadeWarning.run(reinterpret_cast<Nade_t*>(entity));

		switch (cl->m_classID)
		{
		case CInferno:
			drawMolotov(entity);
			break;
		case CPlantedC4:
			drawBomb(entity);
			break;
		case CSmokeGrenadeProjectile:
			drawSmoke(entity);
			break;
		case CFogController:
			runFog(reinterpret_cast<FogController_t*>(entity));
			break;
		case CEnvTonemapController:
			runToneController(reinterpret_cast<EnvTonemapController_t*>(entity));
			break;
		default:
			break;
		}
	}
}

void World::drawBomb(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawBomb))
		return;

	if (Box box; utilities::getBox(ent, box))
		imRender.text(box.x, box.y, ImFonts::tahoma14, XOR("Planted Bomb"), false, Colors::White);
}

// would need better readbality at some point and those offsets for texts are hardcoded
void World::drawBombOverlay()
{
	if (!game::isAvailable())
		m_bombEnt = nullptr;

	if (!m_bombEnt)
		return;

	bool& ref = config.getRef<bool>(vars.bDrawBomb);
	if (!ref)
		return;

	const static auto mp_c4timer = interfaces::cvar->findVar(XOR("mp_c4timer"))->getFloat();
	const auto bombent = reinterpret_cast<Bomb_t*>(m_bombEnt);
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;
	const auto defusetime = m_bombEnt->m_flDefuseCountDown() - interfaces::globalVars->m_curtime;
	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientFromHandle(m_bombEnt->m_hBombDefuser()));
	const auto defuseMaxTime = ent && ent->m_bHasDefuser() ? 5.0f : 10.0f; // check ent too

	if (bombtime < 0.0f || bombent->m_bBombDefused())
	{
		m_bombEnt = nullptr;
		return;
	}

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/389530-bomb-damage-indicator.html
	constexpr float bombRadius = 500.0f; // there is no info for this, run some map scanner
	constexpr float sigma = (500.0f * 3.5f) / 3.0f;
	const float hypDist = (m_bombEnt->getEyePos() - game::localPlayer->getEyePos()).length();
	const float dmg = utilities::scaleDamageArmor((bombRadius * (std::exp(-hypDist * hypDist / (2.0f * sigma * sigma)))), game::localPlayer->m_ArmorValue());
	const bool isSafe = dmg < game::localPlayer->m_iHealth();

	float scaled = m_bombEnt->m_hBombDefuser() > 0 ? (defusetime / defuseMaxTime) : (bombtime / mp_c4timer);

	float fdef = defusetime / defuseMaxTime;
	float fbomb = bombtime / mp_c4timer;
	float forRed = m_bombEnt->m_hBombDefuser() > 0 ? fdef : fbomb;
	float forGreen = m_bombEnt->m_hBombDefuser() > 0 ? fdef : fbomb;

	float r = (255.0f - forRed * 255.0f);
	float g = (forGreen * 255.0f);
	Color color{ static_cast<int>(r), static_cast<int>(g), 0, 200 };

	constexpr ImVec2 size = { 300, 150 };
	ImGui::SetNextWindowSize(size);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, U32(config.get<CfgColor>(vars.cBombBackground).getColor()));
	if (ImGui::Begin(XOR("Bomb c4"), &ref, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		imRenderWindow.addList();

		constexpr float width = size.x;
		constexpr float height = size.y;
		constexpr float radius = height / 3.0f;

		constexpr float scaledX = (width * 0.95f) - radius;
		constexpr float scaledY = height / 2.0f;

		imRenderWindow.drawProgressRing(scaledX, scaledY, radius, 32, -90, scaled, 5.0f, color);
		if(defusetime > bombtime)
			imRenderWindow.drawText(scaledX, scaledY - (radius / 2.5f / 2.0f), radius / 2.5f, ImFonts::franklinGothic30, XOR("Too late"), true, Colors::White, false);
		else
			imRenderWindow.drawText(scaledX, scaledY - (radius / 2.5f / 2.0f), radius / 2.5f, ImFonts::franklinGothic30, FORMAT(XOR("{:.2f}"),
				m_bombEnt->m_hBombDefuser() > 0 ? defusetime : bombtime), true, Colors::White, false);
		imRenderWindow.drawText(2.0f, 0.0f, height / 2.0f, ImFonts::icon, u8"\uE031"_u8str, false, Colors::White, false);
		if (!m_whoPlanted.empty())
			imRenderWindow.drawText(2.0f, height / 2.0f + 2, 15, ImFonts::franklinGothic30, FORMAT(XOR("Planted by {}s"), m_whoPlanted), false, Colors::White, false);
		if(m_bombEnt->m_hBombDefuser() > 0)
			imRenderWindow.drawText(2.0f, height / 2.0f + 18, 15, ImFonts::franklinGothic30, FORMAT(XOR("Defusing {}"), ent->getName()), false, Colors::White, false);
		imRenderWindow.drawText(width / 2.0f, 2.0f, 15, ImFonts::franklinGothic30, FORMAT(XOR("Site {}"), m_bombEnt->getBombSiteName()), true, Colors::White, false);
		imRenderWindow.drawText(width / 2.0f, 20.0f, 15, ImFonts::franklinGothic30, FORMAT(XOR("Damage {:.2f}"), dmg), true, isSafe ? Colors::Green : Colors::Red, false);

		imRenderWindow.end();
		ImGui::End();
	}
	ImGui::PopStyleColor();
}

#include "player.hpp"
#include "../../../SDK/ILocalize.hpp"

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

	if (std::string_view projectileName = studio->m_name; (projectileName.find(XOR("thrown")) != std::string::npos || goodID(id)))
	{
		if (config.get<bool>(vars.bDrawProjectiles))
		{
			std::pair<std::string, CfgColor> nades;

			switch (nadeWarning.getIndexByClass(id, studio))
			{
			case WEAPON_FLASHBANG:
			{
				nades = { XOR("FLASHBANG"), config.get<CfgColor>(vars.cFlashBang) };
				break;
			}
			case WEAPON_HEGRENADE:
			{
				if(wpn->m_nExplodeEffectTickBegin() < 1) // prevent too long time
					nades = { XOR("GRENADE"), config.get<CfgColor>(vars.cGranede) };
				break;
			}
			case WEAPON_MOLOTOV:
			{
				nades = { XOR("MOLOTOV"), config.get<CfgColor>(vars.cMolotov) };
				break;
			}
			case WEAPON_INCGRENADE:
			{
				nades = { XOR("FIRE INC"), config.get<CfgColor>(vars.cIncediary) };
				break;
			}
			case WEAPON_SMOKEGRENADE:
			{
				if (!reinterpret_cast<Smoke_t*>(wpn)->m_nSmokeEffectTickBegin()) // prevent too long time
					nades = { XOR("SMOKE"), config.get<CfgColor>(vars.cSmoke) };
				break;
			}
			case WEAPON_DECOY:
			{
				nades = { XOR("DECOY"), config.get<CfgColor>(vars.cDecoy) }; // this nade time is also too long, check velocity if it's very low
				break;
			}
			case WEAPON_NONE: // understand as NADE_NONE
				return;
			}

			if (Box box; utilities::getBox(ent, box))
				imRender.text(box.x + box.w / 2, box.y + box.h + 2, ImFonts::verdana12, nades.first, true, nades.second.getColor());
		}
	}
	else if (projectileName.find(XOR("dropped")) != std::string::npos && id != CPlantedC4) // add more if needed
	{
		if (Box box; utilities::getBox(ent, box) && config.get<bool>(vars.bDrawDropped))
		{
			float fontSize = visuals.getScaledFontSize(ent, 60.0f, 11.0f, 16.0f);
			using cont = std::vector<bool>; // container
			float padding = 0.0f;

			auto getTextSize = [=](const std::string& text, ImFont* font = ImFonts::verdana12)
			{
				return font->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());
			};

			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::BOX))) // startpoint - no pad
			{
				CfgColor cfgCol = config.get<CfgColor>(vars.cDrawDropped);

				imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Colors::Black);
				imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Colors::Black);
				imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol.getColor());
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::AMMO)) && !wpn->isNonAimable()) // no pad font logic, we just draw extra box
			{
				constexpr float startPad = 3.0f;

				imRender.drawRectFilled(box.x - 1.0f, box.y + box.h - 1.0f + startPad, box.w + 2.0f, 4.0f, Colors::Black);
				imRender.drawRectFilled(box.x, box.y + box.h + startPad,
					wpn->m_iClip1() * box.w / wpn->getWpnInfo()->m_maxClip1, 2.0f, config.get<CfgColor>(vars.cDrawDropped).getColor());

				padding += 4.0f;
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::TEXT)))
			{
				auto name = config.get<bool>(vars.bDrawWeaponTranslate)
					? interfaces::localize->findAsUTF8(wpn->getWpnInfo()->m_WeaponName)
					: wpn->getWpnName();
				imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::verdana12,
					name, true, config.get<CfgColor>(vars.cDropped).getColor());

				auto textSize = getTextSize(name);
				padding += textSize.y;
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::ICON)))
			{
				auto name = utilities::u8toStr(wpn->getIcon());
				imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::icon,
					name, true, config.get<CfgColor>(vars.cDropped).getColor());

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

	// bool to point true = custom
	std::pair<int, bool> keySky;
	if (auto custom = config.get<int>(vars.iCustomSkyBox); custom > 0)
		keySky = { custom, true };
	else
		keySky = { config.get<int>(vars.iSkyBox), false };

	if (keySky.first != 0 && !isShutdown) // is not none and there is no shutdown
	{
		keySky.second == true 
			? loadSkyBoxFunction(m_allCustomSkyboxes.at(keySky.first).c_str())
			: loadSkyBoxFunction(selections::skyboxes.at(keySky.first));
	}
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

	auto editColor = [=](const CfgColor& color)
	{
		*r = color.getColor().r();
		*g = color.getColor().g();
		*b = color.getColor().b();
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
		editColor(config.get<CfgColor>(vars.cWorldTexture));
		isGoodMat = true;
	}

	if (name == XOR("StaticProp textures"))
	{
		editColor(config.get<CfgColor>(vars.cWorldProp));
		isGoodMat = true;
	}

	if (name == XOR("SkyBox textures"))
	{
		editColor(config.get<CfgColor>(vars.cSkyBox));
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
	CfgColor col = config.get<CfgColor>(vars.cMolotovRange);

	//std::vector<ImVec2> points;

	for (int i = 0; i < molotov->m_fireCount(); i++)
	{
		auto pos = origin + molotov->getInfernoPos(i);
		imRender.drawCircle3DFilled(pos, molotovRadius, 32, col.getColor(), col.getColor());

		/*ImVec2 posw;
		if (!imRender.worldToScreen(pos, posw))
			break;

		points.push_back(posw);*/

		//imRender.text(posw.x, posw.y, ImFonts::tahoma, std::to_string(i), false, Colors::Cyan);*/
		// points are not on edge, this will need some graph path logic, and will be done soon
	}
	static float size = ImFonts::tahoma14->FontSize;
	// timer
	if (Vector2D s; imRender.worldToScreen(origin, s))
	{
		imRender.drawProgressRing(s.x, s.y, 25, 32, -90.0f, scale, 5.0f, Colors::LightBlue);
		imRender.text(s.x, s.y - (size / 2.0f), ImFonts::tahoma14, FORMAT(XOR("{:.2f}s"), time), true, Colors::White);
	}

	/*auto g = math::giftWrap(points);
	imRender.drawPolyGon(g, Colors::LightBlue);*/ // still too less points to make is "cool"
}

void drawArc3DSmoke(const Vector& pos, const float radius, const int points, const float percent, const Color& color, const ImDrawFlags flags, const float thickness,
	ImFont* font, const std::string& text, const Color& colorText)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	ImVec2 _end = {};
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f * percent); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector worldEnd = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (ImVec2 start, end; imRender.worldToScreen(worldStart, start) && imRender.worldToScreen(worldEnd, end))
		{
			_end = end;
			imRender.drawLine(start, end, color);
		}
	}

	if (_end.x != 0.0f && _end.y != 0.0f)
		imRender.text(_end.x + 3.0f, _end.y, font, text, true, colorText);
}

void World::drawSmoke(Entity_t* ent)
{
	if (!config.get<bool>(vars.bDrawSmoke))
		return;

	auto smoke = reinterpret_cast<Smoke_t*>(ent);
	if (!smoke)
		return;

	if (!smoke->m_nSmokeEffectTickBegin())
	{
		drawCustomSmoke(ent->absOrigin(), 30.0f);
		return;
	}

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
	drawArc3DSmoke(origin, smokeRadius, 512, scale, config.get<CfgColor>(vars.cDrawSmoke).getColor(), false, 2.0f, ImFonts::tahoma14, FORMAT(XOR("{:.2f}s"), time), Colors::Orange);
	drawCustomSmoke(origin, smokeRadius);

	// timer
	/*if (Vector2D s; imRender.worldToScreen(origin, s))
	{
		imRender.drawProgressRing(s.x, s.y, 25, 32, -90, scale, 5.0f, Colors::LightBlue);
		imRender.text(s.x, s.y, ImFonts::tahoma, std::format(XOR("{:.2f}s"), time), false, Colors::White);
	}*/
}

void World::drawZeusRange()
{
	if (!game::isAvailable())
		return;

	const static auto sv_party_mode = interfaces::cvar->findVar(XOR("sv_party_mode"));
	config.get<bool>(vars.bZeusPartyMode) ? sv_party_mode->setValue(true) : sv_party_mode->setValue(false);

	if (!config.get<bool>(vars.bDrawZeusRange))
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_TASER)
	{
		const static float range = weapon->getWpnInfo()->m_range;
		const Vector abs = game::localPlayer->absOrigin() + Vector(0.0f, 0.0f, 30.0f); // small correction to get correct trace visually, will still throw false positives on stairs etc...

		CfgColor color = config.get<CfgColor>(vars.cZeusRange);

		if (config.get<bool>(vars.bZeusUseTracing))
			imRender.drawCircle3DTraced(abs, range, 32, game::localPlayer(), color.getColor(), true, 2.5f);
		else
			imRender.drawCircle3D(abs, range, 32, color.getColor(), true, 2.0f);
	}
}

void World::drawMovementTrail()
{
	static Vector end;

	if (!game::isAvailable())
		return;

	int type = config.get<int>(vars.iRunMovementTrail);

	if (!config.get<bool>(vars.bRunMovementTrail))
		return;

	if (type != E2T(MovementTrail::BEAM))
	{
		// prepare the point to be corrected
		end = game::localPlayer->m_vecOrigin();
	}

	CfgColor color = config.get<CfgColor>(vars.cMovementTrail);

	switch (type)
	{
	case E2T(MovementTrail::BEAM):
	{
		if (!game::localPlayer->isMoving()) // do not add beams on not moving
			return;

		const Vector start = game::localPlayer->m_vecOrigin();

		BeamInfo_t info = {};

		info.m_type = TE_BEAMPOINTS;
		info.m_modelName = XOR("sprites/purplelaser1.vmt");
		info.m_modelIndex = -1;
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
		info.m_red = color.getColor().rMultiplied();
		info.m_green = color.getColor().gMultiplied();
		info.m_blue = color.getColor().bMultiplied();
		info.m_speed = config.get<float>(vars.fMovementBeamSpeed);
		info.m_startFrame = 0.0f;
		info.m_frameRate = 0.0f;
		info.m_segments = 2;
		info.m_renderable = true;

		auto myBeam = interfaces::beams->createBeamPoints(info);			

		// change to pos after beam is drawn, since it's static it's possible
		end = start;

		break;
	}
	case E2T(MovementTrail::LINE):
	{
		float curtime = interfaces::globalVars->m_curtime;

		if (game::localPlayer->isMoving())
			m_trails.emplace_back(Trail_t{ game::localPlayer->m_vecOrigin(), curtime + config.get<float>(vars.fMovementLife), color.getColor() });

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

			if (ImVec2 start, end; !last.isZero() && imRender.worldToScreen(el.m_pos, start) && imRender.worldToScreen(last, end))
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

void World::clientSideImpacts()
{
	if (!config.get<bool>(vars.bDrawClientSideImpacts))
		return;

	if (!game::isAvailable())
		return;

	auto m_vecBulletVerifyListClient = *reinterpret_cast<CUtlVector<clientHitVerify_t>*>((uintptr_t)game::localPlayer() + 0x11C50);
	static int gameBulletCount = m_vecBulletVerifyListClient.m_size; // init current count

	for (int i = m_vecBulletVerifyListClient.m_size; i > gameBulletCount; i--) // get current bullets, NOT all
		m_hitsClientSide.emplace_back(hitStruct_t
			{
				m_vecBulletVerifyListClient[i - 1].m_pos,
				interfaces::globalVars->m_curtime + config.get<float>(vars.fDrawClientSideImpacts)
			});

	if (m_vecBulletVerifyListClient.m_size != gameBulletCount)
		gameBulletCount = m_vecBulletVerifyListClient.m_size;

	CfgColor outline = config.get<CfgColor>(vars.cDrawClientSideImpactsLine);
	CfgColor fill = config.get<CfgColor>(vars.cDrawClientSideImpactsFill);

	for (size_t i = 0; const auto & el : m_hitsClientSide)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitsClientSide.erase(m_hitsClientSide.begin() + i);
			continue;
		}	

		imRender.drawBox3DFilled(el.m_pos, { 4.0f, 4.0f }, 4.0f,
			outline.getColor(), fill.getColor());

		i++;
	}
}

void World::localImpacts()
{
	if (!config.get<bool>(vars.bDrawLocalSideImpacts))
		return;

	CfgColor outline = config.get<CfgColor>(vars.cDrawLocalSideImpactsLine);
	CfgColor fill = config.get<CfgColor>(vars.cDrawLocalSideImpactsFill);

	for (size_t i = 0; const auto & el : m_hitsLocal)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitsLocal.erase(m_hitsLocal.begin() + i);
			continue;
		}

		Trace_t tr;
		TraceFilter filter;
		filter.m_skip = game::localPlayer();
		interfaces::trace->traceRay({ el.m_start, el.m_end }, MASK_PLAYER, &filter, &tr);

		imRender.drawBox3DFilled(tr.m_end, { 4.0f, 4.0f }, 4.0f, 
			outline.getColor(), fill.getColor());

		i++;
	}
}

void World::removeBloodSpray(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::localPlayer)
		return;

	constexpr std::array arr = // r_cleardecals 
	{
		"decals/flesh/blood1_subrect",
		"decals/flesh/blood2_subrect",
		"decals/flesh/blood3_subrect",
		"decals/flesh/blood4_subrect",
		"decals/flesh/blood5_subrect",
		"decals/flesh/blood6_subrect",
		"decals/flesh/blood8_subrect",
		"decals/flesh/blood9_subrect",
	};

	for (const auto name : arr)
	{
		auto mat = interfaces::matSys->findMaterial(name, XOR(TEXTURE_GROUP_DECAL));
		mat->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, config.get<bool>(vars.bRemoveBloodSpray));
	}
}

// not really removal, more like a "helper"
void World::removeSmoke(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::localPlayer)
		return;

	const static auto throughSmoke = utilities::patternScan(CLIENT_DLL, GOES_THROUGH_SMOKE);
	const static auto smokeCount = *reinterpret_cast<uintptr_t*>(throughSmoke + 0x8);

	if (config.get<bool>(vars.bEditEffectsSmoke)) // remove effects from inside, this is why we nulling smoke count
		*reinterpret_cast<size_t*>(smokeCount) = 0;
}

void World::drawCustomSmoke(const Vector& pos, float radius)
{
	// clockwise for better effect
	Vector end = { radius * std::sin(interfaces::globalVars->m_curtime) + pos.x, radius * std::cos(interfaces::globalVars->m_curtime) + pos.y, pos.z };

	interfaces::effects->smoke(end, -1, 5.0f, 1.0f);
}

void World::pushLocalImpacts(const hitStructLocal_t& hit)
{
	if (!config.get<bool>(vars.bDrawLocalSideImpacts))
		return;

	m_hitsLocal.push_back(hit);
}

bool World::checkCustomSkybox()
{
	auto path = std::filesystem::current_path() / XOR("csgo") / XOR("materials") / XOR("skybox");

	if (!std::filesystem::exists(path))
	{
		if (!std::filesystem::create_directories(path))
			return false;
	}

	m_pathCustomSkybox = path;

	return true;
}

void World::reloadCustomSkyboxes()
{
	m_allCustomSkyboxes.clear();
	m_allCustomSkyboxes.emplace_back(XOR("none"));
	auto iterator = std::filesystem::directory_iterator(m_pathCustomSkybox);
	for (const auto& entry : iterator)
	{
		if (std::string name = entry.path().filename().string();
			(entry.path().extension() == XOR(".vmt") || entry.path().extension() == XOR(".vtf")) && !name.empty())
		{
			m_allCustomSkyboxes.push_back(name);
		}
	}
}

bool World::initSkyboxes()
{
	checkCustomSkybox();
	reloadCustomSkyboxes();

	return true;
}

static constexpr uint32_t U32RGB(const SDKColor& clr)
{
	return (clr.r & 0x0FF) | ((clr.g & 0x0FF) << 8) | ((clr.b & 0x0FF) << 16);
}

void World::runFog(FogController_t* ent)
{
	if (!ent)
		return;

	if (!game::isAvailable())
		return;

	if (globals::isShutdown)
	{
		ent->m_fogenable() = false;
		return;
	}

	if (bool opt = config.get<bool>(vars.bFog); opt)
		ent->m_fogenable() = opt;
	else
	{
		ent->m_fogenable() = opt;
		return;
	}

	SDKColor col = config.get<CfgColor>(vars.cFog).getColor();

	ent->m_fogstart() = 0.0f;
	ent->m_fogend() = config.get<float>(vars.fFogDistance) * 10.0f;
	ent->m_fogmaxdensity() = col.a / 100.0f;
	ent->m_fogcolorPrimary() = U32RGB(col);
	ent->m_fogcolorSecondary() = U32RGB(col);
}

#include "../../../SDK/IMatRenderContext.hpp"
#include "../../menu/GUI-ImGui/selections.hpp"

void World::initEffects()
{
	// warning is that we push "none" too, but below you can see it will never be the case
	// toprevent just cfg + 1 (without pushing "none")
	// for motion blur there is needed old code for this to make it "good"
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/viewpostprocess.cpp#L2996
	for (const std::string_view el : selections::screenEffects)
	{
		bool mark = false;
		if (el == "effects/nightvision") // this looks bad, dunno
			mark = true;

		m_materials.emplace_back(std::make_pair(interfaces::matSys->findMaterial(
			el.data(), TEXTURE_GROUP_CLIENT_EFFECTS), mark));
	}
}

void World::drawEffects()
{
	if (!game::isAvailable())
		return;

	int cfg = config.get<int>(vars.iScreenEffect);
	if (cfg == 0)
		return;

	Color color = config.get<CfgColor>(vars.cScreenEffect).getColor();

	auto material = m_materials.at(cfg).first;
	material->colorModulate(color); // -> works for night vision
	auto var = material->findVar(XOR("$c0_x"));
	var->setValue(config.get<float>(vars.fScreenEffectParam));

	if (m_materials.at(cfg).second) //nightvisioson
		game::localPlayer->m_flNightVisionAlpha() = color.a();

	// now there should be if for motion blur as it needs much more than just drawing this material

	auto ctx = interfaces::matSys->getRenderContext();
	ctx->drawScreenSpaceRectangle(material, 0, 0, globals::screenX, globals::screenY, 0, 0,
		globals::screenX, globals::screenY, globals::screenX, globals::screenY);
	ctx->release();
}

void World::runToneController(EnvTonemapController_t* ent)
{
	if (!game::isAvailable())
		return;
	
	// this is needed to ONLY change when we changed anything
	if (auto cfg = config.get<bool>(vars.bControlTone); cfg && (m_checkStateSlider || m_checkStateButton))
	{
		ent->m_bUseCustomAutoExposureMin() = cfg;
		ent->m_bUseCustomAutoExposureMax() = cfg;
		ent->m_flCustomAutoExposureMin() = config.get<float>(vars.fControlToneMin);
		ent->m_flCustomAutoExposureMax() = config.get<float>(vars.fControlToneMax);
	}
	else if (globals::isShutdown || (!cfg && m_checkStateButton))
	{
		ent->m_bUseCustomAutoExposureMin() = cfg;
		ent->m_bUseCustomAutoExposureMax() = cfg;
	}
}

#include "../../../SDK/ClientClass.hpp"
#include "../../../SDK/IBaseClientDll.hpp"
#include "../../../SDK/ICollideable.hpp"
//#include "../../../SDK/CEffectData.hpp"

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/418432-precipitation-effect-similar-llamahook.html
// 55 8B EC 83 EC 74 56 57 8B F9 8B 0D ? ? ? ? 8B B1 waterripple although r_RainCheck should do it
void World::weather(int stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	// when enable/shutdown
	auto reset = [this]()
	{
		if (m_weather.m_created)
		{
			m_weather.m_created = false;
			if (auto w = m_weather.m_ent)
			{
				w->release();
				w = nullptr;
			}
		}
	};

	auto getNetworkable = [this]() -> Entity_t*
	{
		for (auto _class = interfaces::client->getAllClasses(); _class && !m_weather.m_preciptation; _class = _class->m_next)
		{
			if (_class->m_classID == CPrecipitation)
				m_weather.m_preciptation = _class;
		}

		auto ent = reinterpret_cast<Entity_t*>(m_weather.m_preciptation->m_createFn(MAX_EDICTS - 1, Random::getRandom<int>(0x0, 0xFFF)));
		if (!ent)
			return nullptr;

		return ent;
	};

	// it will only actually reset when the rain is created
	if (globals::isShutdown || !config.get<bool>(vars.bWeather))
	{
		reset();
		return;
	}

	// reduce calling it often
	if (!m_weather.m_created)
	{
		if (!getNetworkable())
			return;

		m_weather.m_ent = reinterpret_cast<Entity_t*>(interfaces::entList->getClientEntity(MAX_EDICTS - 1));
		if (!m_weather.m_ent)
			return;

		constexpr float halfP = Vector::MAX_ARG / 2.0f;
		constexpr float halfM = -Vector::MAX_ARG / 2.0f;

		m_weather.m_ent->m_nPrecipType() = PRECIPITATION_TYPE_SNOW;

		m_weather.m_ent->preDataUpdate(DATA_UPDATE_CREATED);
		m_weather.m_ent->onPreDataChanged(DATA_UPDATE_CREATED);

		m_weather.m_ent->collideable()->OBBMins() = { halfM, halfM, halfM };
		m_weather.m_ent->collideable()->OBBMaxs() = { halfP, halfP, halfP };

		m_weather.m_ent->onDataChanged(DATA_UPDATE_CREATED);
		m_weather.m_ent->postDataUpdate(DATA_UPDATE_CREATED);

		m_weather.m_created = true;
	}
}

void World::implMenu() const
{
	const static auto cvarLenght = interfaces::cvar->findVar(XOR("r_rainlength"));
	const static auto cvarRainSpeed = interfaces::cvar->findVar(XOR("r_rainspeed"));
	const static auto cvarRadius = interfaces::cvar->findVar(XOR("r_rainradius"));
	const static auto cvarWidth = interfaces::cvar->findVar(XOR("r_rainwidth"));
	const static auto cvarSidevel = interfaces::cvar->findVar(XOR("r_RainSideVel"));
	const static auto cvarAlpha = interfaces::cvar->findVar(XOR("r_rainalpha"));
	const static auto cvarWindSpeed = interfaces::cvar->findVar(XOR("cl_windspeed"));

	const static std::array defaults =
	{
		std::make_pair(cvarLenght, cvarLenght->getFloat()),
		std::make_pair(cvarRainSpeed, cvarRainSpeed->getFloat()),
		std::make_pair(cvarRadius, cvarRadius->getFloat()),
		std::make_pair(cvarWidth, cvarWidth->getFloat()),
		std::make_pair(cvarSidevel, cvarSidevel->getFloat()),
		std::make_pair(cvarAlpha, cvarAlpha->getFloat()),
		std::make_pair(cvarWindSpeed, cvarWindSpeed->getFloat()),
	};

	if (ImGui::SliderFloat(XOR("r_rainlength"), &config.getRef<float>(vars.fWeatherRainLenght), 0.0f, 1.0f))
	{
		cvarLenght->setValue(config.get<float>(vars.fWeatherRainLenght));
	}
	if (ImGui::SliderFloat(XOR("r_rainspeed"), &config.getRef<float>(vars.fWeatherRainSpeed), 0.0f, 1000.0f))
	{
		cvarRainSpeed->setValue(config.get<float>(vars.fWeatherRainSpeed));
	}
	if (ImGui::SliderFloat(XOR("r_rainradius"), &config.getRef<float>(vars.fWeatherRainRadius), 0.0f, 3000.0f))
	{
		cvarRadius->setValue(config.get<float>(vars.fWeatherRainRadius));
	}
	if (ImGui::SliderFloat(XOR("r_rainwidth"), &config.getRef<float>(vars.fWeatherRainWidth), 0.0f, 5.0f))
	{
		cvarWidth->setValue(config.get<float>(vars.fWeatherRainWidth));
	}
	if (ImGui::SliderFloat(XOR("r_RainSideVel"), &config.getRef<float>(vars.fWeatherRainSideVel), 0.0f, 1000.0f))
	{
		cvarSidevel->setValue(config.get<float>(vars.fWeatherRainSideVel));
	}
	if (ImGui::SliderFloat(XOR("r_rainalpha"), &config.getRef<float>(vars.fWeatherRainAlpha), 0.0f, 1.0f))
	{
		cvarAlpha->setValue(config.get<float>(vars.fWeatherRainAlpha));
	}
	if (ImGui::SliderFloat(XOR("cl_windspeed"), &config.getRef<float>(vars.fWeatherWindSpeed), 0.0f, 1000.0f))
	{
		cvarWindSpeed->setValue(config.get<float>(vars.fWeatherWindSpeed));
	}
	if (ImGui::Button(XOR("Default rain cvars")))
	{
		for (const auto [convar, defVal] : defaults)
			convar->setValue(defVal);
	}
}

#include "../../../SDK/CViewSetup.hpp"
#include "../../../SDK/ITexture.hpp"
//#define MAX_SPLITSCREEN_PLAYERS 1

// 1:1 from https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/viewpostprocess.cpp#L2996
// with small reduce of code because we dont care for portal/ps3 stuff and also we dont care for detecting blur, but knowing it's enabled
bool World::doImageSpaceMotionBlur(CViewSetup* view)
{
	if (!config.get<bool>(vars.bMotionBlur))
		return false;

	if (!game::isAvailable())
		return false;

	if (!game::localPlayer->isAlive())
		return false;

	constexpr auto resetArr = [](std::array<float, 4>& arr, size_t indexesToReset)
	{
		for (size_t i = 0; auto & el : arr)
		{
			if (i == indexesToReset)
				break;

			el = 0.0f;

			i++;
		}
	};

	if (view)
	{
		float timeElapsed = interfaces::globalVars->m_realtime - m_motionHistory.m_lastTimeUpdate;

		float currentPitch = view->m_angles.x;
		while (currentPitch > 180.0f)
			currentPitch -= 360.0f;
		while (currentPitch < -180.0f)
			currentPitch += 360.0f;

		float currentYaw = view->m_angles.y;
		while (currentYaw > 180.0f)
			currentYaw -= 360.0f;
		while (currentYaw < -180.0f)
			currentYaw += 360.0f;

		Vector currentForwardVec;
		Vector currentSideVec;
		Vector uselessVec;
		math::angleVectors(view->m_angles, currentForwardVec, currentSideVec, uselessVec);

		Vector currentPosition = view->m_origin;
		Vector positionChange = m_motionHistory.m_previousPositon - currentPosition;

		if ((positionChange.length() > 30.0f) && (timeElapsed >= 0.5f))
		{
			resetArr(m_motionBlurValues, m_motionBlurValues.size());
		}
		else if (timeElapsed > (1.0f / 15.0f))
		{
			resetArr(m_motionBlurValues, m_motionBlurValues.size());
		}
		else if (positionChange.length() > 50.0f)
		{
			m_motionHistory.m_noRotationalMotionBlurUntil = interfaces::globalVars->m_realtime + 1.0f;
		}
		else
		{
			const float horizontalFov = view->m_fov;
			const float verticalFov = (view->m_aspectRatio <= 0.0f) ? (view->m_fov) : (view->m_fov / view->m_aspectRatio);
			const float viewDotMotion = currentForwardVec.dot(positionChange);

			if (true)
				m_motionBlurValues[2] = viewDotMotion;
			else
				m_motionBlurValues[2] = viewDotMotion * std::abs(currentForwardVec[2]);

			const float sideDotMotion = currentSideVec.dot(positionChange);
			float yawDiffOriginal = m_motionHistory.m_previousYaw - currentYaw;
			if (((m_motionHistory.m_previousYaw - currentYaw > 180.0f) || (m_motionHistory.m_previousYaw - currentYaw < -180.0f)) &&
				((m_motionHistory.m_previousYaw + currentYaw > -180.0f) && (m_motionHistory.m_previousYaw + currentYaw < 180.0f)))
				yawDiffOriginal = m_motionHistory.m_previousYaw + currentYaw;

			float yawDiffAdjusted = yawDiffOriginal + (sideDotMotion / 3.0f);

			if (yawDiffOriginal < 0.0f)
				yawDiffAdjusted = std::clamp(yawDiffAdjusted, yawDiffOriginal, 0.0f);
			else
				yawDiffAdjusted = std::clamp(yawDiffAdjusted, 0.0f, yawDiffOriginal);

			const float undampenedYaw = yawDiffAdjusted / horizontalFov;
			m_motionBlurValues[0] = undampenedYaw * (1.0f - (std::abs(currentPitch) / 90.0f));

			const float pitchCompensateMask = 1.0f - ((1.0f - std::abs(currentForwardVec[2])) * (1.0f - std::abs(currentForwardVec[2])));
			const float pitchDiffOriginal = m_motionHistory.m_previousPitch - currentPitch;
			float pitchdiffAdjusted = pitchDiffOriginal;

			if (currentPitch > 0.0f)
				pitchdiffAdjusted = pitchDiffOriginal - ((viewDotMotion / 2.0f) * pitchCompensateMask);
			else
				pitchdiffAdjusted = pitchDiffOriginal + ((viewDotMotion / 2.0f) * pitchCompensateMask);

			if (pitchDiffOriginal < 0.0f)
				pitchdiffAdjusted = std::clamp(pitchdiffAdjusted, pitchDiffOriginal, 0.0f);
			else
				pitchdiffAdjusted = std::clamp(pitchdiffAdjusted, 0.0f, pitchDiffOriginal);

			m_motionBlurValues[1] = pitchdiffAdjusted / verticalFov;
			m_motionBlurValues[3] = undampenedYaw;
			m_motionBlurValues[3] *= (std::abs(currentPitch) / 90.0f) * (std::abs(currentPitch) / 90.0f) * (std::abs(currentPitch) / 90.0f);

			if (timeElapsed > 0.0f)
				m_motionBlurValues[2] /= timeElapsed * 30.0f;
			else
				m_motionBlurValues[2] = 0.0f;

			m_motionBlurValues[2] = std::clamp((fabsf(m_motionBlurValues[2]) - config.get<float>(vars.fMotionBlurFallingMin)) / (config.get<float>(vars.fMotionBlurFallingMax) - config.get<float>(vars.fMotionBlurFallingMin)), 0.0f, 1.0f) * (m_motionBlurValues[2] >= 0.0f ? 1.0f : -1.0f);
			m_motionBlurValues[2] /= 30.0f;
			m_motionBlurValues[0] *= config.get<float>(vars.fMotionBlurRotationIntensity) * config.get<float>(vars.fMotionBlurGlobalStrength);
			m_motionBlurValues[1] *= config.get<float>(vars.fMotionBlurRotationIntensity) * config.get<float>(vars.fMotionBlurGlobalStrength);
			m_motionBlurValues[2] *= config.get<float>(vars.fMotionBlurFallingIntensity) * config.get<float>(vars.fMotionBlurGlobalStrength);
			m_motionBlurValues[3] *= config.get<float>(vars.fMotionBlurRollIntensity) * config.get<float>(vars.fMotionBlurGlobalStrength);

			float slowFps = 30.0f;
			float fastFps = 50.0f;
			float currentFps = (timeElapsed > 0.0f) ? (1.0f / timeElapsed) : 0.0f;
			float dumpenFactor = std::clamp(((currentFps - slowFps) / (fastFps - slowFps)), 0.0f, 1.0f);

			m_motionBlurValues[0] *= dumpenFactor;
			m_motionBlurValues[1] *= dumpenFactor;
			m_motionBlurValues[2] *= dumpenFactor;
			m_motionBlurValues[3] *= dumpenFactor;
		}

		if (interfaces::globalVars->m_realtime < m_motionHistory.m_noRotationalMotionBlurUntil)
		{
			resetArr(m_motionBlurValues, 3);
		}
		else
		{
			m_motionHistory.m_noRotationalMotionBlurUntil = 0.0f;
		}

		m_motionHistory.m_previousPositon = currentPosition;
		m_motionHistory.m_previousPitch = currentPitch;
		m_motionHistory.m_previousYaw = currentYaw;
		m_motionHistory.m_lastTimeUpdate = interfaces::globalVars->m_realtime;

		static ITexture* _rt_FullFrameFB = interfaces::matSys->findTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);

		int x = view->x;
		int y = view->y;
		int w = view->m_width;
		int h = view->m_height;

		float srcWidth = static_cast<float>(_rt_FullFrameFB->getActualWidth());
		float srcHeight = static_cast<float>(_rt_FullFrameFB->getActualHeight());
		int offset;
		offset = (x > 0) ? 1 : 0;
		m_motionBlurViewportValues[0] = static_cast<float>(x + offset) / (srcWidth - 1);

		offset = (x < (srcWidth - 1)) ? -1 : 0;
		m_motionBlurViewportValues[3] = static_cast<float>(x + w + offset) / (srcWidth - 1);

		offset = (y > 0) ? 1 : 0;
		m_motionBlurViewportValues[1] = static_cast<float>(y + offset) / (srcHeight - 1);

		offset = (y < (srcHeight - 1)) ? -1 : 0;
		m_motionBlurViewportValues[2] = static_cast<float>(y + h + offset) / (srcHeight - 1);

		for (auto& el : m_motionBlurViewportValues)
		{
			if (el <= 0.0f)
				el = -1.0f;
			else if (el >= 1.0f)
				el = 2.0f;
		}

		return true;
	}

	return false;
}

void World::renderMotionBlur()
{
	if (!config.get<bool>(vars.bMotionBlur))
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	static IMaterial* motion_blur = interfaces::matSys->findMaterial(XOR("dev/motion_blur"), XOR(TEXTURE_GROUP_RENDER_TARGET), false);

	static IMaterialVar* MotionBlurInternal = motion_blur->findVar(XOR("$MotionBlurInternal"), nullptr, false);

	MotionBlurInternal->setVectorComponent(m_motionBlurValues[0], 0);
	MotionBlurInternal->setVectorComponent(m_motionBlurValues[1], 1);
	MotionBlurInternal->setVectorComponent(m_motionBlurValues[2], 2);
	MotionBlurInternal->setVectorComponent(m_motionBlurValues[3], 3);

	static IMaterialVar* MotionBlurViewPortInternal = motion_blur->findVar(XOR("$MotionBlurViewportInternal"), nullptr, false);

	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[0], 0);
	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[1], 1);
	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[2], 2);
	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[3], 3);

	// this code will make fullscreen blured, you probably don't want it. Also group will fuck up everything
	// TEXTURE_GROUP_CLIENT_EFFECTS will pass
	/*auto ctx = interfaces::matSys->getRenderContext();
	ctx->drawScreenSpaceRectangle(motion_blur, 0, 0, globals::screenX, globals::screenY, 0, 0,
		globals::screenX, globals::screenY, globals::screenX, globals::screenY);
	ctx->release();*/

	// so we have to wrap it and call directly in isdepth hook to prevent problems like weapon being blurred
	const static auto _call = utilities::patternScan(CLIENT_DLL, DRAW_SPACE_RECTANGLE_CALL);
	__asm
	{
		push globals::screenY
		push globals::screenX
		push 0
		xor edx, edx
		mov ecx, motion_blur
		call _call
		add esp, 12
	}
}
