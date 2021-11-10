#include "menu.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../../config/config.hpp"
#include "../game.hpp"

static short index = 0;

void Menu::handleKeys(Item* set) const
{
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

	if (LF(GetAsyncKeyState)(VK_INSERT) & 1)
	{
		vars::bMenuOpen = !vars::bMenuOpen;
		config::save();
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

Color handleWhite(int id, int curridx) // so we know when to show darker
{
	if (id == curridx)
		return Color(128, 128, 128, 128);
	return Colors::White;
}

void Menu::drawText(int x, int y, Color color, const std::string& text)
{
	render::text(x, y, fonts::tahoma, std::cref(text), false, color);
}

void Menu::drawBool(int idx, int x, int y)
{
	drawText(x, y, handleWhite(idx, index), settings[idx].getName());
	drawText(x + 100, y, (settings[idx].getBool()) ? Colors::LightBlue : Colors::White, (settings[idx].getBool()) ? "ON" : "OFF");
}

void Menu::drawInt(int idx, int x, int y)
{
	drawText(x, y, handleWhite(idx, index), settings[idx].getName());
	drawText(x + 100, y, Colors::White, std::to_string(settings[idx].getInt()));
}

void Menu::drawVec(int idx, int x, int y)
{
	drawText(x, y, handleWhite(idx, index), settings[idx].getName());
	drawText(x + 100, y, (settings[idx].getInt() != 0) ? Colors::LightBlue : Colors::White, settings[idx].getVec().at(settings[idx].getInt()));
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

	// must have something unique to catch index selected
	// practise with static index for menu is just not good,
	// maybe you want selected index option
	drawBool(0, width, 15);
	drawVec(1, width, 30);
	drawVec(2, width, 45);
	drawBool(3, width, 60);
	drawBool(4, width, 75);
	drawInt(5, width, 90);
	drawBool(6, width, 105);
	drawVec(7, width, 120);
	drawInt(8, width, 135);
	drawInt(9, width, 150);
	drawInt(10, width, 165);
	drawVec(11, width, 180);
	drawBool(12, width, 195);
	drawInt(13, width, 210);
	drawBool(14, width, 225);
	drawInt(15, width, 240);

	width = x * 0.26f;
	drawBool(16, width, 15);
	drawBool(17, width, 30);
	drawBool(18, width, 45);
	drawBool(19, width, 60);
	drawBool(20, width, 75);
	drawVec(21, width, 90);
}
