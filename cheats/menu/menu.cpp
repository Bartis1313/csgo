#include "menu.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../../config/config.hpp"
#include "../game.hpp"

static short index = 0;

#ifndef _DEBUG
void Menu::handleKeys(Item* set) const
{
	// let insert to be controlled 24/7, others only if menu is opened,
	// TODO: add guards for debug detection since writing dummy cached is an overkill
	if (LF(GetAsyncKeyState).cached()(VK_INSERT) & 1)
	{
		vars::bMenuOpen = !vars::bMenuOpen;
		config::save();
	}

	if (!vars::bMenuOpen)
		return;

	if (LF(GetAsyncKeyState).cached()(VK_DOWN) & 1)
	{
		if (index != Item::count - 1)
		{
			index++;
		}
		else
			index = 0;
	}

	if (LF(GetAsyncKeyState).cached()(VK_UP) & 1)
	{
		if (index != 0)
		{
			index--;
		}
		else
			index = Item::count - 1;
	}

	if (LF(GetAsyncKeyState).cached()(VK_RIGHT) & 1)
	{
		if (set[index].isBoolOption())
		{
			set[index].changeBoolRef();
			config::save();
		}
		if (set[index].isAddable())
		{
			set[index].chnageAddableRefp();
			config::save();
		}
		if (set[index].isVectorOption())
		{
			set[index].chnageIntRefp();
			config::save();
		}
	}

	if (LF(GetAsyncKeyState).cached()(VK_LEFT) & 1)
	{
		if (set[index].isBoolOption())
		{
			set[index].changeBoolRef();
			config::save();
		}
		if (set[index].isAddable())
		{
			set[index].chnageAddableRefm();
			config::save();
		}
		if (set[index].isVectorOption())
		{
			set[index].chnageIntRefm();
			config::save();
		}
	}
}
#else
void Menu::handleKeys(Item* set) const
{
	// let insert to be controlled 24/7, others only if menu is opened,
	// TODO: add guards for debug detection since writing dummy cached is an overkill
	if (LF(GetAsyncKeyState)(VK_INSERT) & 1)
	{
		vars::bMenuOpen = !vars::bMenuOpen;
		config::save();
	}

	if (!vars::bMenuOpen)
		return;

	if (LF(GetAsyncKeyState)(VK_DOWN) & 1)
	{
		if (index != Item::count - 1)
		{
			index++;
		}
		else
			index = 0;
	}

	if (LF(GetAsyncKeyState)(VK_UP) & 1)
	{
		if (index != 0)
		{
			index--;
		}
		else
			index = Item::count - 1;
	}

	if (LF(GetAsyncKeyState)(VK_RIGHT) & 1)
	{
		if (set[index].isBoolOption())
		{
			set[index].changeBoolRef();
			config::save();
		}
		if (set[index].isAddable())
		{
			set[index].chnageAddableRefp();
			config::save();
		}
		if (set[index].isVectorOption())
		{
			set[index].chnageIntRefp();
			config::save();
		}
	}

	if (LF(GetAsyncKeyState)(VK_LEFT) & 1)
	{
		if (set[index].isBoolOption())
		{
			set[index].changeBoolRef();
			config::save();
		}
		if (set[index].isAddable())
		{
			set[index].chnageAddableRefm();
			config::save();
		}
		if (set[index].isVectorOption())
		{
			set[index].chnageIntRefm();
			config::save();
		}
	}
}
#endif

Color handleWhite(int id, int curridx) // so we know when to show darker
{
	if (id == curridx)
		return Colors::Grey;
	return Colors::White;
}

void Menu::drawText(int x, int y, Color color, const std::string& text)
{
	render::text(x, y, fonts::tahoma, std::cref(text), false, color);
}

void Menu::drawBool(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), settings[idx].getName());
	drawText(x + 100, y, (settings[idx].getBool()) ? Colors::LightBlue : Colors::White, (settings[idx].getBool()) ? "ON" : "OFF");

	y += 15;
}

void Menu::drawInt(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), settings[idx].getName());
	drawText(x + 100, y, Colors::White, std::to_string(settings[idx].getInt()));

	y += 15;
}

void Menu::drawVec(int idx, int x, int& y)
{
	drawText(x, y, handleWhite(idx, index), settings[idx].getName());
	drawText(x + 100, y, (settings[idx].getInt() != 0) ? Colors::LightBlue : Colors::White, settings[idx].getVec().at(settings[idx].getInt()));

	y += 15;
}

void Menu::draw()
{
	if (!vars::bMenuOpen)
		return;

	int x, y;
	interfaces::surface->getScreenSize(x, y);

	// this is on the left, garbage scaling
	// TODO: some proper, fast and easy scaling with using cvar hudscale iirc
	int width = x * 0.16f;
	int height = 15;

	// must have something unique to catch index selected
	// practise with static index for menu is just not good,
	// maybe you want selected index option
	drawBool(vars::names["bunnyhop"], width, height);
	drawVec(vars::names["chams"], width, height);
	drawVec(vars::names["esp"], width, height);
	drawBool(vars::names["glow"], width, height);
	drawBool(vars::names["soundesp"], width, height);
	drawInt(vars::names["fov"], width, height);
	drawBool(vars::names["thirdp"], width, height);
	drawVec(vars::names["aimbot"], width, height);
	drawInt(vars::names["aimbot_fov"], width, height);
	drawInt(vars::names["smooth"], width, height);
	drawInt(vars::names["rcs"], width, height);
	drawVec(vars::names["xhair"], width, height);
	drawBool(vars::names["triggerbot"], width, height);
	drawInt(vars::names["triggerbot_delay"], width, height);
	drawBool(vars::names["backtrack"], width, height);
	drawInt(vars::names["backtrack_ms"], width, height);

	width = x * 0.26f;
	height = 15;
	drawBool(vars::names["radar"], width, height);
	drawBool(vars::names["local_info"], width, height);
	drawBool(vars::names["esp_flags"], width, height);
	drawBool(vars::names["esp_info"], width, height);
	drawBool(vars::names["esp_skeleton"], width, height);
	drawVec(vars::names["bt_chams"], width, height);
}