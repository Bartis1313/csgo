#pragma once
#include <string>
#include <vector>
#include "Item.hpp"
#include "vars.hpp"
#include "../../SDK/Color.hpp"
#include "../../utilities/singleton.hpp"

class Menu : public singleton<Menu>
{
public:
	Menu() = default;
	void draw();
	void drawText(int x, int y, Color color, const std::string& text);
	void drawBool(int idx, int x, int& y);
	void drawInt(int idx, int x, int& y);
	void drawVec(int idx, int x, int& y);
	void drawOption(int idx, int x, int& y);
	void handleKeys(Item* set) const;
private:
	Item* settings = vars::options;
};