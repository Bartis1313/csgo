#include "misc.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../aimbot/aimbot.hpp"
#include <format>
#include <deque>

constexpr int VK_VKEY = 0x56;

// TODO: rewrite this
void misc::thirdperson()
{
	if (!vars::bThirdp)
		return;

	if (!interfaces::engine->isInGame())
		return;

	if (!game::localPlayer || !game::localPlayer->isAlive())
		return;

	static bool bRun = false;
#ifndef _DEBUG
	if (LF(GetAsyncKeyState).cached()(VK_VKEY) & 1)
		bRun = !bRun;
#else
	if (LF(GetAsyncKeyState)(VK_VKEY) & 1)
		bRun = !bRun;
#endif	
	interfaces::input->m_cameraInThirdPerson = bRun;
	interfaces::input->m_cameraOffset.z = 220.0f;
}

enum CrossHairTypes
{
	STATIC = 1,
	RECOIL,
	ENGINE
};

void misc::drawCrosshair()
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

	const auto crosshair = interfaces::console->findVar(XOR("cl_crosshair_recoil"));
	crosshair->setValue(vars::iCrosshair == ENGINE ? true : false);

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

void misc::drawLocalInfo()
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

	int width, height;
	interfaces::engine->getScreenSize(width, height);
	width *= 0.6f;

	render::text(width, 15, fonts::tahoma, std::format(XOR("Map: {}"), interfaces::engine->getLevelName()), false, Colors::Green);
	render::text(width, 25, fonts::tahoma, std::format(XOR("Weapon {} [{} / {}]"), weapon->getWpnName(), weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount()), false, Colors::Yellow);
	render::text(width, 35, fonts::tahoma, std::format(XOR("Current In-accuracy {:.2f}%"), weapon->getInaccuracy() * 100), false, Colors::Yellow);
	render::text(width, 45, fonts::tahoma, std::format(XOR("Zoom level {}"), weapon->m_zoomLevel()), false, Colors::Yellow);
	render::text(width, 55, fonts::tahoma, std::format(XOR("POS: x {:.2f} y {:.2f} z {:.2f}"), game::localPlayer->absOrigin().x, game::localPlayer->m_vecOrigin().y, game::localPlayer->m_vecOrigin().z), false, Colors::Yellow);
	render::text(width, 65, fonts::tahoma, std::format(XOR("Velocity {:.2f}"), game::localPlayer->m_vecVelocity().Length2D()), false, Colors::Yellow);
	// test
	render::text(width, 75, fonts::tahoma, std::format(XOR("Kills {}"), game::getLocalKills()), false, Colors::Yellow);
	render::text(width, 85, fonts::tahoma, std::format(XOR("Deaths {}"), game::getLocalDeaths()), false, Colors::Yellow);
	// escape divide by zero exceptions by using this trick:
	float kd = game::getLocalKills() / (game::getLocalDeaths() == 0 ? 1 : game::getLocalDeaths());
	render::text(width, 95, fonts::tahoma, std::format(XOR("KD {:.2f}"), kd), false, Colors::Yellow);
	render::text(width, 105, fonts::tahoma, std::format(XOR("Ping {}"), game::getLocalPing()), false, Colors::Yellow);

	width *= 1.25f;
	render::text(width, 15, fonts::tahoma, legitbot::bestEnt ? std::format(XOR("Aimbot working on: {}"), legitbot::bestEnt->getName()) : "", false, Colors::LightBlue);
}

// 1.f / interfaces::globalVars;

struct Record
{
	float fps;
};

void misc::drawFpsGraph()
{
	// because it's not fixed yet
	return;

	// static so we can get records get saved
	static std::deque<Record> records;

	// width of graph
	records.resize(200);

	if (!interfaces::engine->isConnected())
	{
		records.clear();
	}

	int x, y;
	interfaces::engine->getScreenSize(x, y);

	// put every tick to record, this way you don't have to call clear() when heap gets filled more than width of graph
	records.insert(records.begin(), Record{ 1.f / interfaces::globalVars->m_frametime });

	// go for every stored record, -1 to access next record this way
	for (int i = 0; i < records.size() - 1; i++)
	{
		auto current = records.at(i).fps;
		auto next = records.at(i + 1).fps;

		// here this needs to be fixed with getting ratio well
		// TODO: fix this
		RECT points =
		{
			x / 2 - i - 1,
			y / 2 - (current / 3.0f),
			x / 2 - i,
			y / 2 - (next / 3.0f)
		};

		render::drawLine(points.left, points.top, points.right, points.bottom, Color(170, 200, 180, 200));
	}
}
