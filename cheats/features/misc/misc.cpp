#include "misc.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"
#include "../aimbot/aimbot.hpp"
#include "../../globals.hpp"
#include "../../menu/GUI/Misc/guimisc.hpp"
#include <format>
#include <deque>

constexpr int VK_VKEY = 0x56;

// TODO: rewrite this
void misc::thirdperson()
{
	if (!config.get<bool>(vars.bThirdp))
		return;

	if (!interfaces::engine->isInGame())
		return;

	if (!game::localPlayer || !game::localPlayer->isAlive())
		return;

	if (GUI::isKeyPressed(VK_VKEY))
		config.getRef<bool>(vars.bThirdp) = !config.getRef<bool>(vars.bThirdp);

	interfaces::input->m_cameraInThirdPerson = config.get<bool>(vars.bThirdp);
	interfaces::input->m_cameraOffset.z = 220.0f;
}

enum CrossHairTypes
{
	STATIC = 0,
	RECOIL,
	ENGINE
};

void misc::drawCrosshair()
{
	int cfgCross = config.get<int>(vars.iCrosshair);

	const static auto crosshair = interfaces::console->findVar(XOR("cl_crosshair_recoil"));
	crosshair->setValue(cfgCross == ENGINE ? true : false);

	if (cfgCross == -1)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isConnected())
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2;
	y /= 2;

	switch (cfgCross)
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

		if (Vector endScreen; render::worldToScreen(end, endScreen) && game::localPlayer->isAlive())
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
	if (!config.get<bool>(vars.bDrawMiscInfo))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	int width = globals::screenX * 0.6f;

	render::text(width, 15, fonts::tahoma, std::format(XOR("Map: {}"), interfaces::engine->getLevelName()), false, Colors::Green);
	render::text(width, 25, fonts::tahoma, std::format(XOR("Weapon {} [{} / {}]"), weapon->getWpnName(), weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount()), false, Colors::Yellow);
	render::text(width, 35, fonts::tahoma, std::format(XOR("Current In-accuracy {:.2f}%"), weapon->getInaccuracy() * 100), false, Colors::Yellow);
	render::text(width, 45, fonts::tahoma, std::format(XOR("Zoom level {}"), weapon->m_zoomLevel()), false, Colors::Yellow);
	render::text(width, 55, fonts::tahoma, std::format(XOR("POS: x {:.2f} y {:.2f} z {:.2f}"), game::localPlayer->absOrigin().x, game::localPlayer->m_vecOrigin().y, game::localPlayer->m_vecOrigin().z), false, Colors::Yellow);
	render::text(width, 65, fonts::tahoma, std::format(XOR("Velocity {:.2f}"), game::localPlayer->m_vecVelocity().Length2D()), false, Colors::Yellow);

	render::text(width, 75, fonts::tahoma, std::format(XOR("Kills {}"), game::localPlayer->getKills()), false, Colors::Yellow);
	render::text(width, 85, fonts::tahoma, std::format(XOR("Deaths {}"), game::localPlayer->getDeaths()), false, Colors::Yellow);
	// escape divide by zero exceptions by using this trick
	float kd = game::localPlayer->getKills() / (game::localPlayer->getDeaths() ? game::localPlayer->getDeaths() : 1.0f);
	render::text(width, 95, fonts::tahoma, std::format(XOR("KD {:.2f}"), kd), false, Colors::Yellow);
	render::text(width, 105, fonts::tahoma, std::format(XOR("Ping {}"), game::localPlayer->getPing()), false, Colors::Yellow);

	float accuracy = globals::shotsFired
		? (static_cast<float>(globals::shotsHit) / static_cast<float>(globals::shotsFired)) * 100.0f
		: 0.0f;
	render::text(width, 115, fonts::tahoma, std::format(XOR("Accuracy [{} / {}] {:.2f}%"), globals::shotsHit, globals::shotsFired, accuracy), false, Colors::Yellow);

	width *= 1.25f;
	render::text(width, 15, fonts::tahoma, legitbot::bestEnt ? std::format(XOR("Aimbot working on: {}"), legitbot::bestEnt->getName()) : "", false, Colors::LightBlue);
}

struct Record
{
	float fps;
};

void misc::drawFpsPlot()
{
	if (!config.get<bool>(vars.bShowFpsPlot))
		return;

	// static so we can get records get saved
	static std::deque<Record> records;

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2, y /= 2;

	// get position to startdrawing, although this time I am goona start from right pos, because of the loop under
	auto widthPoint = x + (x * 0.97f);

	// a bit of hardcode here
	render::drawFilledRect(widthPoint - records.size() - 1, y - 250 + 2, records.size() + 5, 150, Colors::Grey);
	render::drawOutlineRect(widthPoint - records.size() - 1, y - 250 + 2, records.size() + 5, 150, Colors::Black);

	render::text(widthPoint - records.size() / 2, y - 250 - 14, fonts::tahoma, XOR("FPS Plot"), true, Colors::LightBlue);

	records.push_back(Record{ 1.f / interfaces::globalVars->m_frametime });
	
	// width
	while (records.size() > 300)
		records.pop_front();

	// ratios: put anything you find good
	const auto a = 1.0f;
	const auto b = 10.0f;

	float lx = 0, ly = 0;
	for (int i = 0; i < records.size(); i++)
	{
		// because frames sometimes may be stupid, like on loading new map
		auto currentVal = std::clamp(records.at(i).fps, 0.0f, 500.0f);
		float cx = widthPoint - (i - 1);
		float cy = y - 100 - a * std::sqrt(currentVal * b);

		if (i > 0)
			render::drawLine(cx, cy, lx, ly, Color(170, 200, 180, 200));
		lx = cx; ly = cy;
	}
}

struct RecordVelocity
{
	float velocity;
};

std::deque<RecordVelocity> velRecords;

// because without prediction the values might not be that accurate
void misc::getVelocityData()
{
	if (!config.get<bool>(vars.bShowVelocityPlot))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
	{
		velRecords.clear();
		return;
	}
	
	velRecords.push_back(RecordVelocity{ game::localPlayer->m_vecVelocity().Length2D() });

	// width
	while (velRecords.size() > 300)
		velRecords.pop_front();
}

void misc::drawVelocityPlot()
{
	if (!config.get<bool>(vars.bShowVelocityPlot))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2, y /= 2;

	auto widthPoint = x + (x * 0.97f);

	render::drawFilledRect(widthPoint - velRecords.size() - 1, y - 50 + 2, velRecords.size() + 5, 50, Colors::Grey);
	render::drawOutlineRect(widthPoint - velRecords.size() - 1, y - 50 + 2, velRecords.size() + 5, 50, Colors::Black);

	render::text(widthPoint - velRecords.size() / 2, y - 50 - 14, fonts::tahoma, XOR("Velocity Plot"), true, Colors::LightBlue);

	const auto a = 1.0f;
	const auto b = 2.0f;

	float lx = 0, ly = 0;
	for (int i = 0; i < velRecords.size(); i++)
	{
		auto currentVal = velRecords.at(i).velocity;
		float cx = widthPoint - (i - 1);
		float cy = y - a * std::sqrt(currentVal * b);

		if (i > 0)
			render::drawLine(cx, cy, lx, ly, Color(170, 200, 180, 200));
		lx = cx; ly = cy;
	}
}

void misc::playHitmarker(IGameEvent* event)
{
	if (!config.get<bool>(vars.bPlayHitmarker))
		return;

	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker == game::localPlayer)
	{
		globals::shotsHit++;
		// new hit = new hitmarker
		hitAlpha = interfaces::globalVars->m_curtime;
		// browse those files for more
		interfaces::surface->playSound(XOR("buttons\\arena_switch_press_02.wav"));
	}	
}

// TODO: Add hitmarker when ent died from local player
void misc::drawHitmarker()
{
	if(!config.get<bool>(vars.bDrawHitmarker))
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2, y /= 2;

	if (hitAlpha > 0)
	{
		// you can add ratio for coords to make this hitmarker 100% like from other games
		render::drawLine(x - 10, y + 10, x - 5, y + 5, Color(255, 255, 255, hitAlpha));
		render::drawLine(x + 10, y + 10, x + 5, y + 5, Color(255, 255, 255, hitAlpha));
		render::drawLine(x - 10, y - 10, x - 5, y - 5, Color(255, 255, 255, hitAlpha));
		render::drawLine(x + 10, y - 10, x + 5, y - 5, Color(255, 255, 255, hitAlpha));

		// feel free to use anything that is more accurate, first multiply was just hardcoded by guessing
		hitAlpha -= 1.8f * (interfaces::globalVars->m_curtime) * 255;
	}
}

void misc::drawNoScope()
{
	if (!config.get<bool>(vars.bNoScope))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (!weapon->isSniper())
		return;

	if (game::localPlayer->m_bIsScoped())
	{
		int x = globals::screenX;
		int y = globals::screenY;
		render::drawLine(x / 2, 0, x / 2, y, Colors::Black);
		render::drawLine(0, y / 2, x, y / 2, Colors::Black);
	}
}
