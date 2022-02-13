#pragma once
#include <string>
#include <vector>
#include "Item.hpp"
#include "../../config/vars.hpp"
#include "../../SDK/Color.hpp"

class Menu
{
public:
	Menu() = default;
	void draw();
	void drawText(int x, int y, Color color, const std::string& text);
	void drawBool(int idx, int x, int& y);
	void drawInt(int idx, int x, int& y);
	void drawFloat(int idx, int x, int& y);
	void drawVec(int idx, int x, int& y);
	void drawOption(int idx, int x, int& y);
	void handleKeys() const;
} inline x88Menu;