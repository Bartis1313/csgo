#include <format>
#include "menuX88.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../../config/config.hpp"
#include "../game.hpp"
#include "../globals.hpp"
#include "GUI/Misc/guimisc.hpp"

static short index = 0;

namespace x88Options
{
	inline Item options[] =
	{
		Item{ XOR("BunnyHop"), &config.getRef<bool>(vars.bBunnyHop) }, // 0
		Item{ XOR("Chams"), &config.getRef<int>(vars.iChams), { XOR("OFF"), XOR("Static"), XOR("Xyz")} }, // 1
		Item{ XOR("Esp"), &config.getRef<int>(vars.iEsp), { XOR("OFF"), XOR("2D"), XOR("2D FILLED"), XOR("3D"), XOR("3D FILLED") } }, // 2
		Item{ XOR("Glow"), &config.getRef<bool>(vars.bGlow) }, // 3
		Item{ XOR("Sound Esp"), &config.getRef<bool>(vars.bSoundEsp) }, // 4
		Item{ XOR("Fov"), &config.getRef<int>(vars.iFOV), {-35.0f, 35.0f } }, // 5
		Item{ XOR("ThirdP"), &config.getRef<bool>(vars.bThirdp) }, // 6
		Item{ XOR("Aimbot"), &config.getRef<int>(vars.iAimbot),  { XOR("OFF"), XOR("Nearest"), XOR("Head"), XOR("Chest") } }, // 7
		Item{ XOR("AimFov"), &config.getRef<float>(vars.fFovAimbot), {0.0f, 15.0f } }, // 8
		Item{ XOR("Smooth"), &config.getRef<float>(vars.fSmooth), { 0.0f, 50.0f } }, // 9
		Item{ XOR("Rcs"), &config.getRef<float>(vars.fRCS), { 0.0f, 100.0f } }, // 10
		Item{ XOR("Xhair"), &config.getRef<int>(vars.iCrosshair), { XOR("OFF"), XOR("Static"), XOR("Recoil"), XOR("Engine") } }, // 11
		Item{ XOR("Triggerbot"), &config.getRef<bool>(vars.bTriggerbot) }, // 12
		Item{ XOR("Trigger Delay"), &config.getRef<float>(vars.fTriggerDelay), { 0.0f, 1000.0f } }, // 13
		Item{ XOR("Backtrack"), &config.getRef<bool>(vars.bBacktrack)}, // 14
		Item{ XOR("Backtrack ms"), &config.getRef<int>(vars.iBacktrackTick), { 1.0f, 200.0f } }, // 15
		Item{ XOR("Radar 2D"), &config.getRef<bool>(vars.bRadar) }, // 16
		Item{ XOR("Local Info"), &config.getRef<bool>(vars.bShowInfo) }, // 17
		Item{ XOR("Draw Flags"), &config.getRef<bool>(vars.bShowFlags) }, // 18
		Item{ XOR("Draw Infos"), &config.getRef<bool>(vars.bDrawInfos) }, // 19
		Item{ XOR("Skeleton"), &config.getRef<bool>(vars.bDrawSkeleton) }, // 20
		Item{ XOR("BT Chams"), &config.getRef<int>(vars.iBacktrackChams), { XOR("OFF"), XOR("Stable"), XOR("Last tick"), XOR("Rainbow") } }, // 21,
		Item{ XOR("DLight"), &config.getRef<bool>(vars.bDLight) }, // 22,
		Item{ XOR("NightMode") , &config.getRef<bool>(vars.bRunNight) }, // 23
		Item{ XOR("Esp Lasers"), &config.getRef<bool>(vars.bEspLasers) }, // 24
		Item{ XOR("Draw Plots"), &config.getRef<bool>(vars.bShowPlots) }, // 25
		Item{ XOR("Hand Chams"), &config.getRef<int>(vars.iHandChams), { XOR("OFF"), XOR("Color"), XOR("No Hands") } }, // 26
		Item{ XOR("Weapon Chams"), &config.getRef<int>(vars.iWeaponChams), { XOR("OFF"), XOR("Color"), XOR("No Weapon") } }, // 27
		Item{ XOR("x88menu"), &config.getRef<bool>(vars.bMenuOpenedx88) }, // 28
	};
}

void Menu::handleKeys() const
{
	if (!config.get<bool>(vars.bMenuOpenedx88))
		return;

	if (GUI::isKeyPressed(VK_DOWN))
	{
		if (index != Item::count - 1)
		{
			index++;
		}
		else
			index = 0;
	}

	if (GUI::isKeyPressed(VK_UP))
	{
		if (index != 0)
		{
			index--;
		}
		else
			index = Item::count - 1;
	}

	if (GUI::isKeyPressed(VK_RIGHT))
	{
		if (x88Options::options[index].isBoolOption())
		{
			x88Options::options[index].changeBool();
			config.save(config.getDefaultConfigName());
		}
		if (x88Options::options[index].isIntOption() || x88Options::options[index].isFloatOption())
		{
			x88Options::options[index].chnageAddableByPlus();
			config.save(config.getDefaultConfigName());
		}
		if (x88Options::options[index].isVectorOption())
		{
			x88Options::options[index].changeVectorIndexByPlus();
			config.save(config.getDefaultConfigName());
		}
	}

	if (GUI::isKeyPressed(VK_LEFT))
	{
		if (x88Options::options[index].isBoolOption())
		{
			x88Options::options[index].changeBool();
			config.save(config.getDefaultConfigName());
		}
		if (x88Options::options[index].isIntOption() || x88Options::options[index].isFloatOption())
		{
			x88Options::options[index].chnageAddableByMinus();
			config.save(config.getDefaultConfigName());
		}
		if (x88Options::options[index].isVectorOption())
		{
			x88Options::options[index].changeVectorIndexByMinus();
			config.save(config.getDefaultConfigName());
		}
	}
}

Color handleWhite(int id, int curridx) // so we know when to show darker
{
	if (id == curridx)
		return Colors::Grey;
	return Colors::White;
}

void Menu::drawText(int x, int y, Color color, const std::string& text)
{
	render::text(x, y, fonts::tahoma, text, false, color);
}

void Menu::drawBool(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), x88Options::options[idx].getName());
	drawText(x + 100, y, (x88Options::options[idx].getBool()) ? Colors::LightBlue : Colors::White, (x88Options::options[idx].getBool()) ? XOR("ON") : XOR("OFF"));

	y += 15;
}

void Menu::drawInt(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), x88Options::options[idx].getName());
	drawText(x + 100, y, Colors::White, std::to_string(x88Options::options[idx].getInt()));

	y += 15;
}

void Menu::drawFloat(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), x88Options::options[idx].getName());
	drawText(x + 100, y, Colors::White, std::format(XOR("{:.2f}"), x88Options::options[idx].getFloat())); // round a bit

	y += 15;
}


void Menu::drawVec(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), x88Options::options[idx].getName());
	drawText(x + 100, y, (x88Options::options[idx].getInt() != 0) ? Colors::LightBlue : Colors::White, x88Options::options[idx].getVec().at(x88Options::options[idx].getInt()));

	y += 15;
}

// if you are here and you are lazy to use this drawVec drawBool etc... here is something for ya
void Menu::drawOption(int idx, int x, int& y)
{
	if (x88Options::options[idx].isIntOption())
		drawInt(idx, x, y);
	if (x88Options::options[idx].isFloatOption())
		drawFloat(idx, x, y);
	if(x88Options::options[idx].isBoolOption())
		drawBool(idx, x, y);
	if (x88Options::options[idx].isVectorOption())
		drawVec(idx, x, y);
}

namespace x88map
{
	inline std::map<std::string, int> names =
	{
		{XOR("bunnyhop"), 0},
		{XOR("chams"), 1},
		{XOR("esp"), 2},
		{XOR("glow"), 3},
		{XOR("soundesp"), 4},
		{XOR("fov"), 5},
		{XOR("thirdp"), 6},
		{XOR("aimbot"), 7},
		{XOR("aimbot_fov"), 8},
		{XOR("smooth"), 9},
		{XOR("rcs"), 10},
		{XOR("xhair"), 11},
		{XOR("triggerbot"), 12},
		{XOR("triggerbot_delay"), 13},
		{XOR("backtrack"), 14},
		{XOR("backtrack_ms"), 15},
		{XOR("radar"), 16},
		{XOR("local_info"), 17},
		{XOR("esp_flags"), 18},
		{XOR("esp_info"), 19},
		{XOR("esp_skeleton"), 20},
		{XOR("bt_chams"), 21},
		{XOR("dl_light"), 22},
		{XOR("nightmode"), 23},
		{XOR("esp_lines"), 24},
		{XOR("plots"), 25},
		{XOR("arm_chams"), 26},
		{XOR("weapon_chams"), 27},
		{XOR("x88_opened"), 28},
	};
}


void Menu::draw()
{
	if (!vars.bMenuOpenedx88)
		return;

	int x, y;
	interfaces::surface->getScreenSize(x, y);

	// this is on the left, garbage scaling
	// TODO: some proper, fast and easy scaling with using cvar hudscale iirc
	int width = x * 0.16f;
	int height = 15;

	// for your name, add raw localplayer check, game::localPlayer might be not fast enough, so check in body, it's due to few frames might be called faster
	if (interfaces::engine->isInGame())
	{
		drawText(width, height, Colors::Yellow, game::localPlayer ? XOR("Hello Bartis :)") : XOR("Hello Undefined :)"));
		height += 15;
		drawText(width, height, Colors::Yellow, std::format(XOR("FAKE: {:.2f}"), globals::fakeAngle.y));
		drawText(width + 100, height, Colors::Yellow, std::format(XOR("REAL: {:.2f}"), globals::realAngle.y));
		auto diff = std::abs(globals::fakeAngle.y - globals::realAngle.y);
		drawText(width + 200, height, (diff > 30) ? Colors::Green : Colors::Red, std::format(XOR("DIFF: {:.2f}"), diff));
		height += 15;
	}

	// must have something unique to catch index selected
	// practise with static index for menu is just not good,
	// maybe you want selected index option
	drawBool(x88map::names[XOR("bunnyhop")], width, height);
	drawVec(x88map::names[XOR("chams")], width, height);
	drawVec(x88map::names[XOR("esp")], width, height);
	drawBool(x88map::names[XOR("glow")], width, height);
	drawBool(x88map::names[XOR("soundesp")], width, height);
	drawInt(x88map::names[XOR("fov")], width, height);
	drawBool(x88map::names[XOR("thirdp")], width, height);
	drawVec(x88map::names[XOR("aimbot")], width, height);
	drawFloat(x88map::names[XOR("aimbot_fov")], width, height);
	drawFloat(x88map::names[XOR("smooth")], width, height);
	drawFloat(x88map::names[XOR("rcs")], width, height);
	drawVec(x88map::names[XOR("xhair")], width, height);
	drawBool(x88map::names[XOR("triggerbot")], width, height);
	drawFloat(x88map::names[XOR("triggerbot_delay")], width, height);
	drawBool(x88map::names[XOR("backtrack")], width, height);
	drawInt(x88map::names[XOR("backtrack_ms")], width, height);

	width = x * 0.25f;
	height = 15;

	if (interfaces::engine->isInGame())
	{
		game::localPlayer
			? render::text(width, height, fonts::tahoma, std::format(XOR("Local Player {:X}"), game::localPlayer->getLiteralAddress()), false, Colors::Yellow)
			: render::text(width, height, fonts::tahoma, XOR("Local Player 0x0"), false, Colors::Yellow);
		height += 30;
	}

	drawBool(x88map::names[XOR("radar")], width, height);
	drawBool(x88map::names[XOR("local_info")], width, height);
	drawBool(x88map::names[XOR("esp_flags")], width, height);
	drawBool(x88map::names[XOR("esp_info")], width, height);
	drawBool(x88map::names[XOR("esp_skeleton")], width, height);
	drawVec(x88map::names[XOR("bt_chams")], width, height);
	drawBool(x88map::names[XOR("dl_light")], width, height);
	drawBool(x88map::names[XOR("nightmode")], width, height);
	drawBool(x88map::names[XOR("esp_lines")], width, height);
	drawBool(x88map::names[XOR("plots")], width, height);
	drawVec(x88map::names[XOR("arm_chams")], width, height);
	drawVec(x88map::names[XOR("weapon_chams")], width, height);
	drawBool(x88map::names[XOR("x88_opened")], width, height);
}