#include "visuals.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../SDK/Enums.hpp"
#include "../../globals.hpp"

#undef min
#undef max

enum boxTypes
{
	OFF = 0,
	BOX2D,
	FILLED2D,
	BOX3D,
	FILLED3D
};

// very simple implementation of what to add as color
inline Color healthBased(Player_t* ent)
{
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

void Esp::draw()
{
	for (int i = 1; i < interfaces::engine->getMaxClients(); i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (!game::localPlayer)
			continue;

		if (game::localPlayer == entity)
			continue;

		if (!entity->isPlayer())
			continue;

		if (!entity->isAlive())
			continue;

		if (entity->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		// draw only when ent is valid, don't remove this if
		if (entity)
		{
			drawPlayer(entity);
			drawInfo(entity);
		}

		const auto cl = entity->clientClass();

		if (!cl)
			continue;

		switch (cl->m_classID)
		{
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

bool Esp::getBox(Entity_t* ent, Box& box)
{
	// pretty much nothing to explain here, using engine and detect mins/maxs
	const auto col = ent->colliedable();
	if (!col)
		return false;

	const Vector min = col->OBBMins();
	const Vector max = col->OBBMaxs();

	std::array<Vector, 8> points =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	if (!points.data())
		return false;

	const matrix3x4_t& tranFrame = ent->m_rgflCoordinateFrame();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

	std::array<Vector, 8> screen = {};

	for (int i = 0; i < 8; i++)
	{
		if (!render::WorldToScreen(math::transformVector(points.at(i), tranFrame), screen.at(i)))
			return false;

		left = std::min(left, screen.at(i).x);
		top = std::min(top, screen.at(i).y);
		right = std::max(right, screen.at(i).x);
		bottom = std::max(bottom, screen.at(i).y);
	}

	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = bottom - top;

	return true;
}

void Esp::renderBox3D(Entity_t* ent, bool fill)
{
	const auto col = ent->colliedable();
	if (!col)
		return;

	const Vector min = col->OBBMins();
	const Vector max = col->OBBMaxs();

	std::array<Vector, 8> points =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),

		Vector(min.x, min.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(max.x, max.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	const matrix3x4_t& tranFrame = ent->m_rgflCoordinateFrame();

	Vector transformed[8] = {};

	for (int i = 0; i < 8; i++)
		VectorTransform(points.at(i), tranFrame, transformed[i]);
	
	render::drawBox3D(transformed, healthBased(reinterpret_cast<Player_t*>(ent)), (fill) ? true : false);
}

/* HINT FOR SELECTED TARGET AIMBOT HIGHLIGHT :
* store best ent index in aimbot, then go back there and add  ? : chain to see if ent->getIndex() is same
*/


void Esp::drawBox2D(const Box& box, Player_t* ent)
{
	render::drawOutlineRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0,  200));
	render::drawOutlineRect(box.x, box.y, box.w, box.h, healthBased(ent));
	render::drawOutlineRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 200));
}

void Esp::drawBox2DFilled(const Box& box, Player_t* ent)
{
	const uint8_t alphaOutline = static_cast<uint8_t>(255 * 0.6f);
	const Color fill{ 0, 0, 0, alphaOutline };

	// first create rectangle then do outlines

	render::drawFilledRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, fill);
	drawBox2D(box, ent);
}

void Esp::drawInfo(Player_t* ent)
{
	if (!vars::bShowFlags)
		return;

	// TODO: recode this at all
	Box box;
	if (getBox(ent, box))
	{
		playerInfo_t pInfo;
		interfaces::engine->getPlayerInfo(ent->getIndex(), &pInfo);

		int size = render::getTextSize(fonts::tahoma, pInfo.name);

		render::text(box.x + box.w / 2 - size / 2, box.y - 5, fonts::tahoma, pInfo.name, false, healthBased(ent));

		render::text(box.x + 3 + box.w, box.y - 20 + box.h, fonts::tahoma, std::format("HP {}", ent->m_iHealth()), false, healthBased(ent));

		if (ent->isC4Owner())
			render::text(box.x - 25 + box.w, box.y + 5 + box.h, fonts::tahoma, "C4", false, healthBased(ent));

		render::text(box.x + 3 + box.w, box.y - 10 + box.h, fonts::tahoma, std::format("Armor {}", ent->m_ArmorValue()), false, healthBased(ent));

		if (ent->getActiveWeapon())
		{
			render::text(box.x + 3 + box.w, box.y + box.h + 10, fonts::tahoma, ent->getActiveWeapon()->getWpnName(), false, Colors::White);
			render::text(box.x + 3 + box.w, box.y + box.h + 20, fonts::tahoma, std::format("Ammo {}", ent->getActiveWeapon()->m_iClip1()), false, Colors::White);
		}
	}
}

void Esp::drawPlayer(Player_t* ent)
{
	if (!vars::iEsp)
		return;

	Box box;

	switch (vars::iEsp)
	{
	case BOX2D:
		if (getBox(ent, box))
			drawBox2D(box, ent);
		break;
	case FILLED2D:
		if (getBox(ent, box))
			drawBox2DFilled(box, ent);
		break;
	case BOX3D:		
		renderBox3D(ent, false);
		break;
	case FILLED3D:
		renderBox3D(ent, true);
		break;
	default:
		return;
	}
	return;
}

enum CrossHairTypes
{
	STATIC = 1,
	RECOIL,
	ENGINE
};

void Esp::drawCrosshair()
{
	if (!vars::iCrosshair)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isConnected())
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x, y;
	interfaces::surface->getScreenSize(x, y);
	x /= 2;
	y /= 2;

	Vector punch = {};

	const auto crosshair = interfaces::console->FindVar(XOR("cl_crosshair_recoil"));
	crosshair->setValue((vars::iCrosshair == ENGINE) ? true : false);

	switch (vars::iCrosshair)
	{
	case STATIC:
		// -1 for y because elselike it will be not aligned
		// imo the best is to always throw w,h as odd num
		render::drawFilledRect(x - 5, y - 1, 11, 3, Colors::Black);
		render::drawFilledRect(x - 1, y - 5, 3, 11, Colors::Black);

		render::drawLine(x - 4, y, x + 5, y, Colors::LightBlue);
		render::drawLine(x, y + 4, x, y - 5, Colors::LightBlue);
		break;
	case RECOIL:
	{
		Vector vAngles;
		interfaces::engine->getViewAngles(vAngles);
		vAngles += game::localPlayer->m_aimPunchAngle() * 2.0f; // cvar

		const auto weapon = game::localPlayer->getActiveWeapon();
		if (!weapon)
			break;

		auto start = game::localPlayer->getEyePos();
		auto end = start + math::angleVec(vAngles) * 10000; // I could not find anything good in all struct for this, hence why hardcode

		// easy crosshair
		//int width = x / 90;
		//int height = y / 90;
		//width -= (width * (game::localPlayer->m_aimPunchAngle().y));
		//height += (height * (game::localPlayer->m_aimPunchAngle().x));

		if (Vector endScreen; render::WorldToScreen(end, endScreen) && game::localPlayer->isAlive())
		{
			render::drawFilledRect(endScreen.x - 5, endScreen.y - 1, 11, 3, Colors::Black);
			render::drawFilledRect(endScreen.x - 1, endScreen.y - 5, 3, 11, Colors::Black);

			render::drawLine(endScreen.x - 4, endScreen.y, endScreen.x + 5, endScreen.y, Colors::LightBlue);
			render::drawLine(endScreen.x, endScreen.y + 4, endScreen.x, endScreen.y - 5, Colors::LightBlue);
		}
		break;
	}
	default:
		break;
	}
}

void Esp::drawBombDropped(Entity_t* ent)
{
	if (!vars::bShowFlags)
		return;

	auto ownerHandle = ent->m_hOwnerEntity();
	auto ownerEntity = interfaces::entList->getClientFromHandle(ownerHandle);

	if (!ownerEntity)
	{
		Vector screen = {};
		if (render::WorldToScreen(ent->absOrigin(), screen))
			render::text(screen.x, screen.y, fonts::tahoma, "Dropped bomb", false, Colors::White);
	}
}

void Esp::drawBomb(Entity_t* const ent)
{
	const auto tickbomb = interfaces::console->FindVar(XOR("mp_c4timer"))->getFloat();
	const auto bombent = reinterpret_cast<Bomb_t*>(ent);
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;

	if (bombtime < 0.0f)
		return;

	Box box;
	if (getBox(ent, box))
	{
		render::text(box.x, box.y, fonts::tahoma, "Planted Bomb", false, Colors::LightBlue);
		render::drawOutlineRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0, 200));
		render::drawOutlineRect(box.x, box.y, box.w, box.h, Color(120, 60, 0, 200));
		render::drawOutlineRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 200));
	}

	uint8_t r = static_cast<uint8_t>(255.0f - bombtime / tickbomb * 255.0f);
	uint8_t g = static_cast<uint8_t>(bombtime / tickbomb * 255.0f);

	render::text(5, 800, fonts::tahoma, std::format("TIME: {:.3f}", bombtime), false, Color{r, g, 0, 200});
}


inline bool Esp::isNade(const int classid)
{
	if (classid == CBaseCSGrenadeProjectile
		|| classid == CDecoyProjectile
		|| classid == CMolotovProjectile)
		return true;
	return false;
}

void Esp::drawProjectiles(Entity_t* const ent)
{
	if (!vars::bShowFlags)
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

	if (projectileName.find("thrown") != std::string::npos
		|| isNade(cl->m_classID))
	{
		Vector nadeOrigin = ent->absOrigin();
		Vector nadePos = {};

		if (!render::WorldToScreen(nadeOrigin, nadePos))
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

		if (getBox(ent, box))
		{
			render::text(box.x + box.w / 2, box.y + box.h + 2, fonts::tahoma, nadeName, false, colorNade);
		}
	}
	else if(projectileName.find("dropped") != std::string::npos)
	{
		const auto drop = reinterpret_cast<Weapon_t*>(ent);

		Box box;

		if (getBox(ent, box))
		{
			render::text(box.x + box.w / 2, box.y + box.h + 2, fonts::tahoma, drop->getWpnName(), false, Colors::White);
		}
	}
}

// add this to events manager 
void Esp::drawSound(IGameEvent* event)
{
	if (!vars::bSoundEsp)
		return;

	auto userid = interfaces::engine->getPlayerID(event->getInt(XOR("userid")));

	auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(userid));

	if (!entity)
		return;

	if (!entity->isPlayer())
		return;

	if (!entity->isAlive())
		return;

	auto modelIndex = interfaces::modelInfo->getModelIndex(XOR("sprites/physbeam.vmt"));

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

	auto beam_draw = interfaces::beams->createRingPoint(info);

	if (beam_draw)
		interfaces::beams->drawBeam(beam_draw);
}

static constexpr auto dotThickness = 5;
static constexpr auto dotRad = 20.0f;

Vector2D Esp::entToRadar(Vector eye, Vector angles, Vector EntityPos, Vector2D pos, Vector2D size, float scale, bool& check)
{
	float directionX, directionY;
	float dotX, dotY;

	directionX = -(EntityPos.y - eye.y);
	directionY = EntityPos.x - eye.x;

	auto yawDeg = angles.y - 90.0f;
	// calculate dots of radian and return correct view
	const auto yawToRadian = DEG2RAD(yawDeg);
	dotX = (directionY * std::cos(yawToRadian) - directionX * std::sin(yawToRadian)) / dotRad;
	dotY = (directionY * std::sin(yawToRadian) + directionX * std::cos(yawToRadian)) / dotRad;
	// return correct scale, it zooms in/out depends what value is thrown
	dotX *= scale;
	dotY *= scale;
	// correct it for our center screen of rectangle radar
	dotX += size.x / 2;
	dotY += size.y / 2;

	// do not draw out of range
	if (dotX < dotThickness)
		check = false;

	if (dotX > size.x)
		check = false;

	if (dotY < dotThickness)
		check = false;

	if (dotY > size.y)
		check = false;

	// again correct for out center...
	dotX += pos.x;
	dotY += pos.y;

	// should be just 2d vec, 3rd z value for 3d is empty because radar is a normal rectnagle -> 2D
	return Vector2D(dotX, dotY);
}

void Esp::radar()
{
	if (!vars::bRadar)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	int x, y;
	interfaces::engine->getScreenSize(x, y);
	int centerx = x / 2 - 20;
	int centery = y - 110;
	int size = 90;

	render::drawTrapezFilled(Vector2D(centerx - size, centery - size), Vector2D(centerx + size, centery - size),
		Vector2D(centerx + size, centery + size), Vector2D(centerx - size, centery + size), Colors::Grey);

	render::drawTrapezOutline(Vector2D(centerx - size - 1, centery - size - 1), Vector2D(centerx + size + 1, centery - size - 1),
		Vector2D(centerx + size + 1, centery + size + 1), Vector2D(centerx - size - 1, centery + size + 1), Colors::Black);

	int toAlign = render::getTextSize(fonts::tahoma, "Radar");
	render::text(centerx - toAlign / 2, centery - size - 10, fonts::tahoma, "Radar", false, Colors::LightBlue);

	render::drawFilledRect(centerx, centery - size, 1, 2 * size, Color(0, 0, 0, 120));
	render::drawFilledRect(centerx - size, centery, 2 * size, 1, Color(0, 0, 0, 120));

	const auto myEye = game::localPlayer->getEyePos();
	Vector ang = {};
	interfaces::engine->getViewAngles(ang);


	for (int i = 1; i < interfaces::globalVars->m_maxClients; i++)
	{
		auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!ent)
			continue;

		if (!game::localPlayer)
			continue;

		if (ent->isDormant())
			continue;

		if (ent == game::localPlayer)
			continue;

		if (!ent->isAlive() || !game::localPlayer->isAlive())
			continue;

		if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		bool check = true;
		const auto entRotatedPos = entToRadar(myEye, ang, ent->absOrigin(), Vector2D(centerx - size, centery - size), Vector2D(2.0f * size, 2.0f * size), 1.8f, check);

		// must be better way
		auto entYaw = ent->m_angEyeAngles().y;

		if (entYaw < 0)
			entYaw = 180 + ((180 + entYaw));

		const auto rotated = (270 - entYaw) + ang.y;

		// this arg should not be dotRad in this case, as value gets higher line too
		const auto finalX = dotRad * std::cos(DEG2RAD(rotated));
		const auto finalY = dotRad * std::sin(DEG2RAD(rotated));

		if (check)
		{
			render::drawLine(entRotatedPos.x - 1, entRotatedPos.y - 1, entRotatedPos.x + finalX, entRotatedPos.y + finalY, Colors::White);
			render::drawCircle(entRotatedPos.x, entRotatedPos.y, dotThickness, 32, Color(254, 24, 110, 255));
		}		
	}
}

//TODO: fix scaling
void Esp::drawLocalInfo()
{
	if (!vars::bShowInfo)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	render::text(1200, 20, fonts::tahoma, std::format("Map: {}", interfaces::engine->getLevelName()), false, Colors::Green);
	render::text(1200, 30, fonts::tahoma, std::format("Weapon {} [{} / {}]", weapon->getWpnName().c_str(), weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount()), false, Colors::Yellow);
	render::text(1200, 40, fonts::tahoma, std::format("Current In-accuracy {:.2f}", weapon->getInaccuracy()), false, Colors::Yellow);
	render::text(1200, 50, fonts::tahoma, std::format("Zoom level {}", weapon->m_zoomLevel()), false, Colors::Yellow);
	render::text(1200, 60, fonts::tahoma, std::format("POS: x: {:.2f} y: {:.2f} z: {:.2f}", game::localPlayer->absOrigin().x, game::localPlayer->m_vecOrigin().y, game::localPlayer->m_vecOrigin().z), false, Colors::Yellow);
	render::text(1200, 70, fonts::tahoma, std::format("Velocity: {:.2f}", game::localPlayer->m_vecVelocity().Length2D()), false, Colors::Yellow);
	// test
	render::text(1200, 80, fonts::tahoma, std::format("Kills: {}", game::getLocalKills()), false, Colors::Yellow);
	render::text(1200, 90, fonts::tahoma, std::format("Deaths: {}", game::getLocalDeaths()), false, Colors::Yellow);
	// escape divide by zero exceptions by using this trick:
	float kd = game::getLocalKills() / (game::getLocalDeaths() == 0) ? 1 : game::getLocalDeaths();
	render::text(1200, 100, fonts::tahoma, std::format("KD: {:.2f}", kd), false, Colors::Yellow);
	render::text(1200, 110, fonts::tahoma, std::format("Ping: {}", game::getLocalPing()), false, Colors::Yellow);
}
