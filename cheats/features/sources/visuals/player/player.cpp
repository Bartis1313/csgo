#include "player.hpp"

#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/IVModelInfo.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/IVEffects.hpp"
#include "../../../SDK/IGameEvent.hpp"
#include "../../../SDK/IViewRenderBeams.hpp"
#include "../../../SDK/animations.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/Color.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../config/vars.hpp"
#include "../../../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"
#include "../../../../globals.hpp"

#include "../../aimbot/aimbot.hpp"
#include "../../backtrack/backtrack.hpp"
#include "boxes.hpp"
#include "enemyWarn.hpp"
#include "sounds.hpp"

void PlayerVisuals::init()
{

}

void PlayerVisuals::draw()
{
	if (!config.get<bool>(vars.bEsp))
		return;

	if (!game::isAvailable())
		return;

	bool drawDead = config.get<bool>(vars.bDrawDeadOnly);

	std::pair<bool, bool> warnChecks = { false, false };

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (entity == game::localPlayer)
			continue;

		if (entity->isDormant())
			continue;

		if (!entity->isAlive())
			continue;

		if (entity->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		auto runFeatures = [=]()
		{
			drawPlayer(entity);
			drawSkeleton(entity);
			runDLight(entity);
			drawLaser(entity);
			g_SoundDraw.findBest(entity);
		};

		if (drawDead)
		{
			if (!game::localPlayer->isAlive())
				runFeatures();
		}
		else
			runFeatures();

		warnChecks = g_EnemyWarning.check(entity);
	}
	g_SoundDraw.draw();
	g_EnemyWarning.draw(warnChecks);
}

void PlayerVisuals::drawHealth(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawHealth))
		return;

	auto& health = m_health.at(ent->getIndex());

	if (auto realHealth = ent->m_iHealth(); health > realHealth)
		health -= 2.0f * interfaces::globalVars->m_frametime;
	else
		health = realHealth;

	auto offset = health * box.h / 100.0f;
	auto pad = box.h - offset;

	Box newBox =
	{
		box.x - 5.0f,
		box.y,
		2.0f,
		box.h,
	};

	// fill first
	imRender.drawRoundedRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, 4.0f, newBox.h + 2.0f, 120.0f, Colors::Black);
	imRender.drawRoundedRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, 120.0f, Color::healthBased(ent->m_iHealth()));

	// if the player has health below max, then draw HP info
	if (health < 100)
	{
		auto text = FORMAT(XOR("{}"), health);
		float fontSize = utilities::getScaledFont(ent->absOrigin(), game::localPlayer->absOrigin(), 100.0f, 10.0f, 14.0f);
		auto size = ImFonts::franklinGothic12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());

		imRender.text(newBox.x - 4.0f - size.x, newBox.y + pad - 4.0f,
			fontSize, ImFonts::franklinGothic12, text, false, Colors::White);
	}
}

void PlayerVisuals::drawArmor(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawArmor))
		return;

	auto& armor = m_armor.at(ent->getIndex());

	if (auto realArmor = ent->m_ArmorValue(); armor > realArmor)
		armor -= 2.0f * interfaces::globalVars->m_frametime;
	else
		armor = realArmor;

	auto offset = armor * box.h / 100.0f;
	auto pad = box.h - offset;

	Box newBox =
	{
		box.x + box.w + 3.0f,
		box.y,
		2.0f,
		box.h,
	};

	Color armorCol = Color(0, static_cast<int>(armor * 1.4f), 255); // light to blue, something simple

	if (armor != 0)
	{
		imRender.drawRoundedRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, 4.0f, newBox.h + 2.0f, 120.0f, Colors::Black);
		imRender.drawRoundedRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, 120.0f, armorCol);
	}

	/*if (armor < 100 && armor != 0)
		imRender.text(newBox.x - 2.0f, newBox.y + pad - 4.0f,
			ImFonts::franklinGothic, std::format(XOR("{}"), armor), false, Colors::White);*/
}

#include "../../../SDK/ILocalize.hpp"

void PlayerVisuals::drawWeapon(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawWeapon))
		return;

	auto weapon = ent->getActiveWeapon();
	if (!weapon)
		return;

	CfgColor tex = config.get<CfgColor>(vars.cWeaponText);

	int maxAmmo = weapon->getWpnInfo()->m_maxClip1;
	int currentAmmo = weapon->m_iClip1();

	imRender.text(box.x + box.w / 2, box.y + box.h + 5, ImFonts::franklinGothic12, FORMAT(XOR("{} {}/{}"),
		config.get<bool>(vars.bDrawWeaponTranslate) ? interfaces::localize->findAsUTF8(weapon->getWpnInfo()->m_WeaponName) : ent->getActiveWeapon()->getWpnName(), currentAmmo, maxAmmo),
		true, tex.getColor());

	// skip useless trash for calculations
	if (weapon->isNonAimable())
		return;

	Box newBox =
	{
		box.x,
		box.y + box.h + 3.0f,
		box.w + 2.0f,
		2.0f
	};

	int barWidth = currentAmmo * box.w / maxAmmo;
	bool isReloading = false;
	auto animlayer = ent->getAnimOverlays()[1];

	if (animlayer.m_sequence)
	{
		auto sequenceActivity = ent->getSequenceActivity(animlayer.m_sequence);
		isReloading = sequenceActivity == 967 && animlayer.m_weight; // ACT_CSGO_RELOAD

		if (isReloading && animlayer.m_weight != 0.0f && animlayer.m_cycle < 0.99f)
			barWidth = (animlayer.m_cycle * box.w) / 1.0f;
	}

	imRender.drawRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, newBox.w, 4.0f, Colors::Black);
	imRender.drawRectFilled(newBox.x, newBox.y, barWidth, 2.0f, config.get<CfgColor>(vars.cReloadbar).getColor());
}

void PlayerVisuals::drawInfo(Player_t* ent, const Box& box)
{
	std::vector<std::pair<std::string, Color>> flags = {};
	using cont = std::vector<bool>; // container

	PlayerInfo_t info = {};
	if (!interfaces::engine->getPlayerInfo(ent->getIndex(), &info))
		return;

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::BOT)))
		if(info.m_fakePlayer)
			flags.emplace_back(std::make_pair(XOR("BOT"), Colors::Yellow));

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::MONEY)))
		flags.emplace_back(std::make_pair(FORMAT(XOR("{}$"), ent->m_iAccount()), Colors::Green));

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::WINS)))
		flags.emplace_back(std::make_pair(FORMAT(XOR("Wins {}"), ent->getWins()), Colors::Green));

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::RANK)))
		flags.emplace_back(std::make_pair(FORMAT(XOR("Rank {}"), ent->getRank()), Colors::White));

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::ARMOR)))
	{
		std::string text = "";
		if (ent->m_bHasHelmet() && ent->m_ArmorValue())
			text = XOR("H KEV");
		else if (!ent->m_bHasHelmet() && ent->m_ArmorValue())
			text = XOR("KEV");

		flags.emplace_back(std::make_pair(text, Colors::White));
	}

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::ZOOM)))
		if(ent->m_bIsScoped())
			flags.emplace_back(std::make_pair(XOR("ZOOM"), Colors::White));

	if (config.get<cont>(vars.vFlags).at(E2T(EspFlags::C4)))
		if (ent->isC4Owner())
			flags.emplace_back(std::make_pair(XOR("C4"), Colors::Orange));

	float fontSize = utilities::getScaledFont(ent->absOrigin(), game::localPlayer()->absOrigin(), 60.0f, 11.0f, 16.0f);

	float padding = 0.0f;
	float addon = config.get<bool>(vars.bDrawArmor) ? 6.0f : 0.0f;

	for (size_t i = 0; const auto & [name, color] : flags)
	{
		imRender.text(box.x + box.w + addon + 2.0f, box.y + padding, fontSize, ImFonts::verdana12, name, false, color, false);
		auto textSize = ImFonts::verdana12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, name.c_str());
		padding += textSize.y;
		i++;

		if (i != flags.size() && padding + fontSize > box.h) // when too many flags for long distances
		{
			imRender.text(box.x + box.w + addon + 2.0f, box.y + padding, fontSize, ImFonts::verdana12,
				FORMAT(XOR("{} more..."), flags.size() - i), false, Colors::White, false);
			break;
		}
	}
}

void PlayerVisuals::drawnName(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawName))
		return;

	float fontSize = utilities::getScaledFont(ent->absOrigin(), game::localPlayer()->absOrigin());

	imRender.text(box.x + box.w / 2.0f, box.y - fontSize - 2.0f, fontSize, ImFonts::verdana12, ent->getName(), true, Color::healthBased(ent->m_iHealth()), false);
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void PlayerVisuals::drawSkeleton(Player_t* ent)
{
	if (!config.get<bool>(vars.bDrawSkeleton))
		return;

	auto model = ent->getModel();
	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	// have to check if selected record is filled, if no then just skip
	auto record = !g_Backtrack.getAllRecords().at(ent->getIndex()).empty() ? &g_Backtrack.getAllRecords().at(ent->getIndex()) : nullptr;
	auto skeletPos = [=](const size_t idx)
	{
		auto child = record != nullptr
			? record->back().m_matrix[idx].origin()
			: ent->getBonePos(idx);
		return child;
	};

	// bone IDs
	constexpr auto chest = 6;
	constexpr auto lowerChest = 5;

	for (int i = 0; i < studio->m_bonesCount; i++)
	{
		auto bone = studio->getBone(i);
		if (!bone)
			continue;

		if (bone->m_parent == -1)
			continue;

		if (!(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		if (record && !g_Backtrack.isValid(record->front().m_simtime)) // if backtrack
			continue;

		auto child = skeletPos(i);
		auto parent = skeletPos(bone->m_parent);
		auto chestbone = skeletPos(chest);
		auto upper = skeletPos(chest + 1) - chestbone;
		auto breast = chestbone + upper / 2.0f;

		auto deltachild = child - breast;
		auto deltaparent = parent - breast;

		if (deltaparent.length() < 9.0f && deltachild.length() < 9.0f)
			parent = breast;

		if (i == lowerChest)
			child = breast;

		if (std::abs(deltachild.z) < 5.0f && deltaparent.length() < 5.0f && deltachild.length() < 5.0f || i == chest)
			continue;

		if (config.get<bool>(vars.bSkeletonDebugPoints))
		{
			if (ImVec2 s; imRender.worldToScreen(ent->getBonePos(i), s))
				imRender.text(s.x, s.y, ImFonts::franklinGothic12, FORMAT(XOR("{}"), i), true, Colors::White, true);
		}

		if (ImVec2 start, end; imRender.worldToScreen(parent, start) && imRender.worldToScreen(child, end))
			imRender.drawLine(start, end, config.get<CfgColor>(vars.cSkeleton).getColor());
	}
}

void PlayerVisuals::drawSnapLine(Player_t* ent, const Box& box)
{
	if (ent == g_Aimbot.getTargetted())
	{
		// lines on the bottom and center bottom box
		imRender.drawLine(globals::screenX / 2, globals::screenY, box.x + box.w / 2, box.y + box.h, Colors::Purple);
	}
}

void PlayerVisuals::drawLaser(Player_t* ent)
{
	if (!config.get<bool>(vars.bEspLasers))
		return;

	// get from where to start, "laser ESP" is always starting from head I think
	auto start = ent->getBonePos(8);
	// get angle to draw with correct view
	auto forward = math::angleVec(ent->m_angEyeAngles());
	// end is where lines just ends, this 70 is hardcoded, but whatever here tbh
	auto end = start + forward * 70.f;

	if (ImVec2 startP, endLine; imRender.worldToScreen(start, startP) && imRender.worldToScreen(end, endLine))
	{
		imRender.drawCircleFilled(startP.x, startP.y, 3, 32, Colors::Red);
		imRender.drawLine(startP, endLine, Colors::Purple);
	}
}

void PlayerVisuals::runDLight(Player_t* ent)
{
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/client/c_spotlight_end.cpp#L112

	if (!config.get<bool>(vars.bDLight))
		return;

	auto dLight = interfaces::efx->clAllocDLight(ent->getIndex());
	dLight->m_color = config.get<CfgColor>(vars.cDlight).getColor();
	dLight->m_origin = ent->m_vecOrigin();
	dLight->m_radius = config.get<float>(vars.fDlightRadius);
	dLight->m_die = interfaces::globalVars->m_curtime + 0.1f;
	dLight->m_exponent = static_cast<char>(config.get<float>(vars.fDlightExponent));
	dLight->m_decay = config.get<float>(vars.fDlightDecay);
	dLight->m_key = ent->getIndex();
}

void PlayerVisuals::drawPlayer(Player_t* ent)
{
	if (!config.get<bool>(vars.bEsp))
		return;

	Box box; Box3D box3d;
	if (!utilities::getBox(ent, box, box3d))
		return;

	switch (config.get<int>(vars.iEsp))
	{
	case E2T(BoxTypes::BOX2D):
		g_BoxesDraw.drawBox2D(box);
		break;
	case E2T(BoxTypes::FILLED2D):
		g_BoxesDraw.drawBox2DFilled(box);
		break;
	case E2T(BoxTypes::BOX3D):
		g_BoxesDraw.drawBox3D(box3d);
		break;
	case E2T(BoxTypes::FILLED3D):
	{
		if (!config.get<bool>(vars.bBoxMultiColor))
			g_BoxesDraw.drawBox3DFilled(box3d);
		else
			g_BoxesDraw.drawBox3DFilledMultiColor(box3d);
		break;
	}
	default:
		break;
	}

	drawHealth(ent, box);
	drawArmor(ent, box);
	drawWeapon(ent, box);
	drawInfo(ent, box);
	drawSnapLine(ent, box);
	drawnName(ent, box);
}

