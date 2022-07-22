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
#include "../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"
#include "../../globals.hpp"

#include "../../features/aimbot/aimbot.hpp"
#include "../../features/backtrack/backtrack.hpp"

void Visuals::run()
{
	if (!config.get<bool>(vars.bEsp))
		return;

	if (!game::isAvailable())
		return;

	bool drawDead = config.get<bool>(vars.bDrawDeadOnly);

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
			findBestSound(entity);
		};

		if (drawDead)
		{
			if (!game::localPlayer->isAlive())
				runFeatures();
		}
		else
			runFeatures();

		enemyIsAimingAtYou(entity);
	}
	drawSound();
}

void Visuals::drawHealth(Player_t* ent, const Box& box)
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
	imRender.drawRoundedRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, 120.0f, healthBased(ent));

	// if the player has health below max, then draw HP info
	if (health < 100)
	{
		auto text = FORMAT(XOR("{}"), health);
		float fontSize = getScaledFontSize(ent, 100.0f, 10.0f, 14.0f);
		auto size = ImFonts::franklinGothic12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());

		imRender.text(newBox.x - 4.0f - size.x, newBox.y + pad - 4.0f,
			fontSize, ImFonts::franklinGothic12, text, false, Colors::White);
	}
}

void Visuals::drawArmor(Player_t* ent, const Box& box)
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

void Visuals::drawWeapon(Player_t* ent, const Box& box)
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

void Visuals::drawInfo(Player_t* ent, const Box& box)
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

	float fontSize = getScaledFontSize(ent, 60.0f, 11.0f, 16.0f);

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

void Visuals::drawnName(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawName))
		return;

	float fontSize = getScaledFontSize(ent);

	imRender.text(box.x + box.w / 2.0f, box.y - fontSize - 2.0f, fontSize, ImFonts::verdana12, ent->getName(), true, healthBased(ent), false);
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void Visuals::drawSkeleton(Player_t* ent)
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
	auto record = !backtrack.getAllRecords().at(ent->getIndex()).empty()  ? &backtrack.getAllRecords().at(ent->getIndex()) : nullptr;
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

		if (record && !backtrack.isValid(record->front().m_simtime)) // if backtrack
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

		if (ImVec2 start, end; imRender.worldToScreen(parent, start) && imRender.worldToScreen(child, end))
			imRender.drawLine(start, end, config.get<CfgColor>(vars.cSkeleton).getColor());
	}
}

void Visuals::drawSnapLine(Player_t* ent, const Box& box)
{
	if (ent == aimbot.getTargetted())
	{
		// lines on the bottom and center bottom box
		imRender.drawLine(globals::screenX / 2, globals::screenY, box.x + box.w / 2, box.y + box.h, Colors::Purple);
	}
}

void Visuals::drawLaser(Player_t* ent)
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

void Visuals::runDLight(Player_t* ent)
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

void Visuals::drawPlayer(Player_t* ent)
{
	if (!config.get<bool>(vars.bEsp))
		return;

	Box box; Box3D box3d;
	if (!utilities::getBox(ent, box, box3d))
		return;

	switch (config.get<int>(vars.iEsp))
	{
	case E2T(BoxTypes::BOX2D):
		drawBox2D(box);
		break;
	case E2T(BoxTypes::FILLED2D):
		drawBox2DFilled(box);
		break;
	case E2T(BoxTypes::BOX3D):
		drawBox3D(box3d);
		break;
	case E2T(BoxTypes::FILLED3D):
	{
		if (!config.get<bool>(vars.bBoxMultiColor))
			drawBox3DFilled(box3d);
		else
			drawBox3DFilledMultiColor(box3d);
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

void Visuals::pushStep(const StepData_t& step)
{
	if (!config.get<bool>(vars.bSoundEsp))
		return;

	m_steps.at(step.m_player->getIndex()).push_back(step);
}

void Visuals::findBestSound(Entity_t* entity)
{
	if (!config.get<bool>(vars.bSoundEsp))
		return;

	int index = entity->getIndex();

	int x = globals::screenX / 2.0f;
	int y = globals::screenY / 2.0f;
	float maxDist = config.get<float>(vars.fStepMaxDist);
	float maxDistLine = config.get<float>(vars.fStepMaxLineDist);

	for (size_t i = 0; const auto & el : m_steps.at(index))
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_steps.at(index).erase(m_steps.at(index).begin() + i);
			continue;
		}

		Vector2D elPos;
		if (!imRender.worldToScreen(el.m_pos, elPos))
			continue;

		//float scale = diff / config.get<float>(vars.fStepTime); // ratio
		float alpha = (maxDist - el.m_pos.distToMeters(game::localPlayer->absOrigin())) / maxDist; // alpha fading per distance
		if (alpha < 0.1f)
			continue;

		if (constexpr float maxDiff = 1.0f; diff < maxDiff) // fading effect
			alpha = (diff / maxDiff) * alpha;

		// again same thing, I don't want to use ent origin but current pos
		auto scaledFont = [=](const float division = 80.0f, const float min = 12.0f, const float max = 30.0f)
		{
			float dist = el.m_pos.distTo(game::localPlayer->absOrigin());
			float fontSize = std::clamp(division / (dist / division), min, max);
			return fontSize;
		};

		float rad = scaledFont(50.0f, 3.0f, 8.0f);
		imRender.drawCircleFilled(elPos.x, elPos.y, rad, 32.0f,
			config.get<CfgColor>(vars.cSoundEsp).getColor().getColorEditAlpha(alpha));

		float distFromMiddle = std::round(std::sqrt((elPos.x - x) * (elPos.x - x) + (elPos.y - y) * (elPos.y - y)));

		if (distFromMiddle < maxDistLine)
		{
			if (!m_bestStep.m_player || distFromMiddle < m_bestStep.m_maxPixels)
			{
				m_bestStep.m_player = el.m_player;
				m_bestStep.m_pos = el.m_pos;
				m_bestStep.m_timeToPrint = diff;
				m_bestStep.m_maxPixels = distFromMiddle;
				// better not
				/*bestStep.m_fontSize = scaledFont(50.0f, 10.0f, 18.0f);*/
			}
		}

		i++;
	}
}

void Visuals::drawSound()
{
	int x = globals::screenX / 2.0f;
	int y = globals::screenY / 2.0f;

	if (m_bestStep.m_player)
	{
		if (Vector2D pos; imRender.worldToScreen(m_bestStep.m_pos, pos))
		{
			std::string_view place = m_bestStep.m_player->m_szLastPlaceName();
			if (place.empty())
				place = XOR("Unknown");
			std::string nameText = FORMAT(XOR("{} -> {} [{:.1f}m]"), m_bestStep.m_player->getName(),
				place, game::localPlayer->absOrigin().distToMeters(m_bestStep.m_pos));
			std::string timeText = FORMAT(XOR("Time left {:.1f}s"), m_bestStep.m_timeToPrint);

			x = globals::screenX / 2.5f;

			float textSize = std::max(
				imRender.getTextSize(ImFonts::tahoma14, timeText).x,
				imRender.getTextSize(ImFonts::tahoma14, nameText).x);

			static float fontSize = ImFonts::tahoma14->FontSize + 2.0f;

			imRender.text(x, y, ImFonts::tahoma14, timeText, false, Colors::White);
			imRender.text(x, y - fontSize, ImFonts::tahoma14, nameText, false, Colors::White);
			imRender.drawLine(x, y, x + textSize, y, config.get<CfgColor>(vars.cStepLine).getColor());
			imRender.drawLine(x + textSize, y, pos.x, pos.y, config.get<CfgColor>(vars.cStepLine).getColor());
		}

		m_bestStep.m_player = nullptr;
	}
}

Color Visuals::healthBased(Player_t* ent)
{
	int health = ent->m_iHealth();
	int g = health * 2.55f;
	int r = 255 - g;
	return Color(r, g, 0, 255);
}

void Visuals::enemyIsAimingAtYou(Player_t* ent)
{
	if (!config.get<bool>(vars.bAimingWarn))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	Vector posDelta = ent->getEyePos() - game::localPlayer->getEyePos();
	Vector idealAimAngle = math::vectorToAngle(posDelta);

	// account for their spray control
	static const auto scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"))->getFloat();
	idealAimAngle -= ent->m_aimPunchAngle() * scale;

	Vector curEnemyAngle = ent->m_angEyeAngles();
	curEnemyAngle.normalize();

	// check trace
	bool check = ent->isPossibleToSee(game::localPlayer->getEyePos());

	// dynamic fov
	float fov = math::calcFovReal(ent->getEyePos(), game::localPlayer->getBonePos(3), curEnemyAngle); // 3 is middle body

	if (check) // no, check it differently
	{
		imRender.text(globals::screenX / 2, 60, ImFonts::tahoma14, XOR("Enemy can see you"), true, Colors::Green);
	}
	// this can be made with tracing
	if (fov <= 5.0f)
	{
		imRender.text(globals::screenX / 2, 80, ImFonts::tahoma14, XOR("Enemy is aiming you"), true, Colors::Red);
	}
}

float Visuals::getScaledFontSize(Entity_t* ent, const float division, const float min, const float max)
{
	float dist = ent->absOrigin().distTo(game::localPlayer->absOrigin());
	// clamping is hardcoded, for csgo there are no huge distances. For other games that have long distances rendered, this will still run ok.
	float fontSize = std::clamp(division / (dist / division), min, max);
	return fontSize;
}

#pragma region drawing_boxes

void Visuals::drawBox2D(const Box& box)
{
	CfgColor cfgCol = config.get<CfgColor>(vars.cBox);
	Color outlineCol = Colors::Black.getColorEditAlpha(cfgCol.getColor().a());
	bool outlined = config.get<bool>(vars.bBoxOutlined);

	if (outlined)
	{
		imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, outlineCol);
		imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, outlineCol);
	}
	imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol.getColor());
}

void Visuals::drawBox2DFilled(const Box& box)
{
	CfgColor fill = config.get<CfgColor>(vars.cBoxFill);

	if(!config.get<bool>(vars.bBoxMultiColor))
		imRender.drawRectFilled(box.x, box.y, box.w, box.h, fill.getColor());
	else
	{
		float speed = config.get<float>(vars.fBoxMultiColor);
		float time = interfaces::globalVars->m_curtime;

		imRender.drawRectFilledMultiColor(box.x, box.y, box.w, box.h,
			Color::rainbowColor(time, speed).getColorEditAlpha(fill.getColor().a()),
			Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(fill.getColor().a()),
			Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(fill.getColor().a()),
			Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(fill.getColor().a()));
	}
	drawBox2D(box);
}

static ImVec2 operator-(const ImVec2& v, float val)
{
	return ImVec2{ v.x - val, v.y - val };
}

static ImVec2 operator+(const ImVec2& v, float val)
{
	return ImVec2{ v.x + val, v.y + val };
}

void Visuals::drawBox3DFilled(const Box3D& box, const float thickness)
{
	CfgColor color = config.get<CfgColor>(vars.cBox);
	CfgColor fill = config.get<CfgColor>(vars.cBoxFill);
	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false; // looks bad on 3d

	auto points = math::grahamScan(box.points);
	std::reverse(points.begin(), points.end());
	imRender.drawPolyGon(points, fill.getColor());

	Color outlineCol = Colors::Black.getColorEditAlpha(color.getColor().a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

void Visuals::drawBox3DFilledMultiColor(const Box3D& box, const float thickness)
{
	CfgColor color = config.get<CfgColor>(vars.cBox);
	CfgColor fill = config.get<CfgColor>(vars.cBoxFill);
	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	float speed = config.get<float>(vars.fBoxMultiColor);
	float time = interfaces::globalVars->m_curtime;

	std::vector colors =
	{
		U32(Color::rainbowColor(time, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 4.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 5.0f, speed).getColorEditAlpha(fill.getColor().a())),
	};

	auto points = math::grahamScan(box.points);
	size_t delta = colors.size() - points.size();
	std::reverse(points.begin(), points.end());
	imRender.drawPolyGonMultiColor(points, colors); // any way to make it smooth trnasmition in vertex? I couldn't think of

	Color outlineCol = Colors::Black.getColorEditAlpha(color.getColor().a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

void Visuals::drawBox3D(const Box3D& box, const float thickness)
{
	CfgColor color = config.get<CfgColor>(vars.cBox);
	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	Color outlineCol = Colors::Black.getColorEditAlpha(color.getColor().a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

#pragma endregion