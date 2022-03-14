#include "player.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../config/vars.hpp"
#include "../../game.hpp"
#include "../../../SDK/Enums.hpp"
#include "../../globals.hpp"
#include "../aimbot/aimbot.hpp"
#include "../backtrack/backtrack.hpp"
#include <format>

#undef min
#undef max

void drawBox3D(const Box3D& box, const Color& color, bool filled, const float thickness = 2.0f);

// very simple implementation of what to add as color
inline Color playerColor(Player_t* ent, bool isAimbotColor = false)
{
	if (legitbot::bestEnt == ent && isAimbotColor)
		return Colors::Purple;

	uint8_t health = ent->m_iHealth();
	uint8_t g = static_cast<uint8_t>(health * 2.55f);
	uint8_t r = 255 - g;
	return Color(
		r,
		g,
		0,
		220
	);
}

// dead esp - true = draw when dead, TODO

void esp::run()
{
	if (!game::localPlayer)
		return;

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (game::localPlayer == entity)
			continue;

		if (entity->isDormant())
			continue;

		if (!entity->isAlive())
			continue;

		if (entity->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		// draw only when ent is valid, don't remove this if
		if (entity)
		{
			drawPlayer(entity);
			drawSkeleton(entity);
			runDLight(entity);
			drawLaser(entity);
			enemyIsAimingAtYou(entity);
		}
	}
}

void esp::renderBox3D(Entity_t* ent, bool fill)
{
	Box3D box;
	if (!utilities::getBox3D(ent, box))
		return;

	drawBox3D(box, Colors::Purple, fill);
}

void drawBox3D(const Box3D& box, const Color& color, bool filled, const float thickness)
{
	// this could be simplified in some loop, maybe...
	if (filled)
	{
		// anything with low alpha - TEMP color! will add optionality later
		Color fill{ color.rMultiplied(), color.gMultiplied(), color.bMultiplied(), 30 };

		imRender.drawQuadFilled(box.points.at(0), box.points.at(1), box.points.at(2), box.points.at(3), fill);
		// top
		imRender.drawQuadFilled(box.points.at(4), box.points.at(5), box.points.at(6), box.points.at(7), fill);
		// front
		imRender.drawQuadFilled(box.points.at(3), box.points.at(2), box.points.at(6), box.points.at(7), fill);
		// back
		imRender.drawQuadFilled(box.points.at(0), box.points.at(1), box.points.at(5), box.points.at(4), fill);
		// right
		imRender.drawQuadFilled(box.points.at(2), box.points.at(1), box.points.at(5), box.points.at(6), fill);
		// left
		imRender.drawQuadFilled(box.points.at(3), box.points.at(0), box.points.at(4), box.points.at(7), fill);
	}

	imRender.drawQuad(box.points.at(0), box.points.at(1), box.points.at(2), box.points.at(3), color, thickness);
	// top
	imRender.drawQuad(box.points.at(4), box.points.at(5), box.points.at(6), box.points.at(7), color, thickness);
	// front
	imRender.drawQuad(box.points.at(3), box.points.at(2), box.points.at(6), box.points.at(7), color, thickness);
	// back
	imRender.drawQuad(box.points.at(0), box.points.at(1), box.points.at(5), box.points.at(4), color, thickness);
	// right
	imRender.drawQuad(box.points.at(2), box.points.at(1), box.points.at(5), box.points.at(6), color, thickness);
	// left
	imRender.drawQuad(box.points.at(3), box.points.at(0), box.points.at(4), box.points.at(7), color, thickness);
}


void esp::drawBox2D(Player_t* ent, const Box& box)
{
	Color cfgCol = config.get<Color>(vars.cBox);

	imRender.drawRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0,  200));
	imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol);
	imRender.drawRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 200));
}

void esp::drawBox2DFilled(Player_t* ent, const Box& box)
{
	Color fill = config.get<Color>(vars.cBoxFill);

	// first create rectangle then do outlines

	imRender.drawRectFilled(box.x - 1, box.y - 1, box.w + 2, box.h + 2, fill);
	drawBox2D(ent, box);
}

void esp::drawHealth(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawHealth))
		return;

	auto health = ent->m_iHealth() > 100 ? 100 : ent->m_iHealth();
	if (health)
	{
		auto offset = health * box.h / 100;
		auto pad = box.h - offset;

		Box newBox =
		{
			box.x - 6,
			box.y - 1,
			2,
			box.h + 2,
		};

		// fill first
		imRender.drawRectFilled(newBox.x, newBox.y, newBox.w, newBox.h, Colors::Black);
		imRender.drawRectFilled(newBox.x, newBox.y + pad - 1, 2, offset + 2, playerColor(ent));
		imRender.drawRect(newBox.x - 1, newBox.y - 1, 4, newBox.h, Colors::Black);

		// if the player has health below max, then draw HP info
		if (health < 100)
			imRender.text(newBox.x - 2, newBox.y + pad - 4, ImFonts::espBar, std::to_string(health), false, Colors::White);
	}
}

void esp::drawArmor(Player_t* ent, const Box& box)
{
	if(!config.get<bool>(vars.bDrawArmor))
		return;

	auto armor = ent->m_ArmorValue() > 100 ? 100 : ent->m_ArmorValue();
	if (armor)
	{
		auto offset = armor * box.h / 100;
		auto pad = box.h - offset;

		Box newBox =
		{
			box.x + box.w + 4,
			box.y - 1,
			2,
			box.h + 2,
		};

		Color armorCol = Color(0, static_cast<int>(armor * 1.4f), 250, 255); // light to blue, something simple

		imRender.drawRectFilled(newBox.x, newBox.y, newBox.w, newBox.h, Colors::Black);
		imRender.drawRectFilled(newBox.x, newBox.y + pad - 1, 2, offset + 2, armorCol);
		imRender.drawRect(newBox.x - 1, newBox.y - 1, 4, newBox.h, Colors::Black);


		if (armor < 100)
			imRender.text(newBox.x - 2, newBox.y + pad - 4, ImFonts::espBar, std::to_string(armor), false, Colors::White);
	}
}

void esp::drawWeapon(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawWeapon))
		return;

	auto weapon = ent->getActiveWeapon();
	if (!weapon)
		return;

	Color tex = config.get<Color>(vars.cWeaponText);

	imRender.text(box.x + box.w / 2, box.y + box.h + 5, ImFonts::espBar, ent->getActiveWeapon()->getWpnName(), true, tex);

	// skip useless trash for calculations
	if (weapon->isNonAimable())
		return;

	int maxAmmo = weapon->getWpnInfo()->m_maxClip1;
	int currentAmmo = weapon->m_iClip1();

	Box newBox =
	{
		box.x,
		box.y + box.h + 3,
		box.w + 2,
		2
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

	imRender.drawRectFilled(newBox.x - 1, newBox.y - 1, newBox.w, 4, Colors::Black);
	imRender.drawRectFilled(newBox.x, newBox.y, barWidth, 2, config.get<Color>(vars.cReloadbar));
	
	if (maxAmmo != currentAmmo && !isReloading)
		imRender.text(newBox.x + barWidth, newBox.y + 1, ImFonts::espBar, std::to_string(currentAmmo), false, tex);
}

void esp::drawInfo(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawInfos))
		return;

	imRender.text(box.x + (box.w / 2), box.y - 15, ImFonts::tahoma, ent->getName(), true, playerColor(ent));

	if (ent->isC4Owner())
		imRender.text(box.x - 25 + box.w, box.y + 5 + box.h, ImFonts::tahoma, XOR("C4"), false, playerColor(ent));
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void esp::drawSkeleton(Player_t* ent)
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
	auto record = !backtrack::records[ent->getIndex()].empty() ? &backtrack::records[ent->getIndex()] : nullptr;

	for (int i = 0; i < studio->m_bonesCount; i++)
	{
		auto bone = studio->bone(i);
		if (!bone)
			continue;

		if (bone->m_parent == -1)
			continue;

		if (!(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// skip like here
		auto child = record
			? record->back().matrix[i].origin()
			: ent->getBonePosition(i);

		auto parent = record
			? record->back().matrix[bone->m_parent].origin()
			: ent->getBonePosition(bone->m_parent);

		auto chest = 6;

		auto upper = record
			? record->back().matrix[chest + 1].origin() - record->back().matrix[chest].origin()
			: ent->getBonePosition(chest + 1) - ent->getBonePosition(chest);

		auto breast = record
			? record->back().matrix[chest].origin() + upper / 2
			: ent->getBonePosition(chest) + upper / 2;

		auto deltachild = child - breast;
		auto deltaparent = parent - breast;

		if (deltaparent.length() < 9.0f && deltachild.length() < 9.0f)
			parent = breast;

		if (i == 5)
			child = breast;

		if (abs(deltachild.z) < 5.0f && deltaparent.length() < 5.0f && deltachild.length() < 5.0f || i == 6)
			continue;

		if (Vector screenp, screenc; imRender.worldToScreen(parent, screenp) && imRender.worldToScreen(child, screenc))
		{
			Color skel = config.get<Color>(vars.cSkeleton);

			if(record && backtrack::isValid(record->front().simTime))
				imRender.drawLine(screenp.x, screenp.y, screenc.x, screenc.y, skel);
			else if(!record)
				imRender.drawLine(screenp.x, screenp.y, screenc.x, screenc.y, skel);
		}
	}
}

void esp::drawSnapLine(Player_t* ent, const Box& box)
{
	if (ent == legitbot::bestEnt)
	{
		// lines on the bottom and center bottom box
		imRender.drawLine(globals::screenX / 2, globals::screenY, box.x + box.w / 2, box.y + box.h, Colors::Purple);
	}
}

void esp::drawLaser(Player_t* ent)
{
	if (!config.get<bool>(vars.bEspLasers))
		return;

	// get from where to start, "laser ESP" is always starting from head I think
	auto start = ent->getBonePosition(8);
	// get angle to draw with correct view
	auto forward = math::angleVec(ent->m_angEyeAngles());
	// end is where lines just ends, this 70 is hardcoded, but whatever here tbh
	auto end = start + forward * 70.f;

	if (Vector screenLocal, screenEnt; imRender.worldToScreen(start, screenLocal) && imRender.worldToScreen(end, screenEnt))
	{
		imRender.drawCircleFilled(screenLocal.x, screenLocal.y, 3, 32, Colors::Red);
		imRender.drawLine(screenLocal.x, screenLocal.y, screenEnt.x, screenEnt.y, Colors::Purple);
	}
}

void esp::runDLight(Player_t* ent)
{
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/client/c_spotlight_end.cpp#L112

	if (!interfaces::engine->isInGame())
		return;

	if (!config.get<bool>(vars.bDLight))
		return;

	auto DLight = interfaces::effects->clAllocDlight(ent->getIndex());
	DLight->m_style = DLIGHT_NO_WORLD_ILLUMINATION;
	DLight->m_color = { config.get<Color>(vars.cDlight) };
	DLight->m_origin = ent->absOrigin();
	DLight->m_radius = 80.0f;
	DLight->m_die = interfaces::globalVars->m_curtime + 0.05f;
	DLight->m_exponent = 10.0f;
	DLight->m_decay = 30.0f;
	DLight->m_key = ent->getIndex();
}

void esp::drawPlayer(Player_t* ent)
{
	if (!config.get<bool>(vars.bEsp))
		return;

	Box box;
	if (!utilities::getBox(ent, box))
		return;

	switch (config.get<int>(vars.iEsp))
	{
	case E2T(BoxTypes::BOX2D):
		drawBox2D(ent, box);
		break;
	case E2T(BoxTypes::FILLED2D):
		drawBox2DFilled(ent, box);
		break;
	case E2T(BoxTypes::BOX3D):
		renderBox3D(ent, false);
		break;
	case E2T(BoxTypes::FILLED3D):
		renderBox3D(ent, true);
		break;
	default:
		break;
	}

	drawHealth(ent, box);
	drawArmor(ent, box);
	drawWeapon(ent, box);
	drawInfo(ent, box);
	drawSnapLine(ent, box);
}

// add this to events manager 
void esp::drawSound(IGameEvent* event)
{
	if (!config.get<bool>(vars.bSoundEsp))
		return;

	auto userid = interfaces::engine->getPlayerID(event->getInt(XOR("userid")));

	auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(userid));

	if (!entity)
		return;

	if (!entity->isPlayer())
		return;

	if (!entity->isAlive())
		return;

	if (entity->m_bGunGameImmunity())
		return;

	static auto modelIndex = interfaces::modelInfo->getModelIndex(XOR("sprites/physbeam.vmt"));

	BeamInfo_t info = {};

	info.m_type = TE_BEAMRINGPOINT;
	info.m_flags = FBEAM_FADEOUT;
	info.m_modelName = XOR("sprites/physbeam.vmt");
	info.m_modelIndex = modelIndex;
	info.m_haloIndex = -1;
	info.m_haloScale = 3.0f;
	info.m_life = 2.0f;
	info.m_width = 4.0f;
	info.m_fadeLength = 1.0f;
	info.m_amplitude = 0.0f;
	info.m_red = 70.f;
	info.m_green = 130.f;
	info.m_blue = 200.f;
	info.m_brightness = 255.f;
	info.m_speed = 0.0f;
	info.m_startFrame = 0.0f;
	info.m_frameRate = 60.0f;	
	info.m_vecCenter = entity->absOrigin() + Vector(0.0f, 0.0f, 5.0f);
	info.m_startRadius = 5.0f;
	info.m_endRadius = 20.f;
	info.m_renderable = true;

	if (auto beamDraw = interfaces::beams->createRingPoint(info); beamDraw)
		interfaces::beams->drawBeam(beamDraw);
}

void esp::enemyIsAimingAtYou(Player_t* ent)
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
	static const auto scale = interfaces::console->findVar(XOR("weapon_recoil_scale"))->getFloat();
	idealAimAngle -= ent->m_aimPunchAngle() * scale;

	Vector curEnemyAngle = ent->m_angEyeAngles();

	curEnemyAngle.normalize();

	float dy = math::normalizeYaw(curEnemyAngle.y - idealAimAngle.y),
		dp = curEnemyAngle.x - idealAimAngle.x;

	float fovDist = std::sqrt(dy * dy + dp * dp);
	bool check = ent->isPossibleToSee(game::localPlayer->getEyePos());

	// hardcoded, when enemies use 3rd cam or some fov changer, it won't be accurate
	if (check && fovDist <= 60.0f)
	{
		imRender.text(globals::screenX / 2, 60, ImFonts::tahoma, XOR("Enemy can see you"), true, Colors::Green);
	}
	// in the moment when enemy aims through walls, don't check trace
	if (fovDist <= 5.0f)
	{
		imRender.text(globals::screenX / 2, 80, ImFonts::tahoma, XOR("Enemy is aiming you"), true, Colors::Red);
	}
}