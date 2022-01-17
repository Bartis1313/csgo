#include "gui.hpp"
#include <map>

GUI::KeyHolder::KeyHolder(const std::string& title, const int x, const int y, const int width, const int height,
	const Color& color, const Color& secColor, int* key)
	: m_title{ title }, m_color{ color }, m_secColor{ secColor }, m_key{ key }
{
	setPos(x, y, width, height);
}

// some banned keys
constexpr bool isbadKey(const short key)
{
	if (key == VK_ESCAPE || key == VK_RETURN
		|| key == VK_INSERT || key == VK_PRINT
		|| key == 0x5B || key == 0x5C) // windows keys
		return true;

	return false;
}

void GUI::KeyHolder::draw()
{
	// only on start
	std::string name = "init";
	static bool isGettingKey = false;

	// if key is given, then change name to "..." waiting state
	if (isGettingKey)
	{
		name = "...";

		// looop through possible choices
		for (short i = 1; i < 256; i++)
		{
			if (isKeyPressed(i))
			{
				// is bad key? then throw some unknown index
				isbadKey(i) ? *m_key = -1 : *m_key = i;
				isGettingKey = false;
			}
		}
	}
	else
	{
		// if it's valid key?
		if (*m_key)
		{
			// assign starting name
			name = utilities::getKeyName(*m_key);
		}
	}

	// to align better? yes
	Vector2D textSize = render::getTextSizeXY(fonts::menuFont, name);

	bool isInRange = isMouseInRange(globals::menuX + m_X - textSize.x, globals::menuY + m_Y, textSize.x, textSize.y + 3);

	if (isKeyPressed(VK_LBUTTON) && isInRange)
	{
		isGettingKey = true;
	}

	// isGettingKey can be this->isActive() in this case, soon it will be patched with extras

	render::drawFilledRect(globals::menuX + m_X - textSize.x, globals::menuY + m_Y, textSize.x + 6, textSize.y + 3,
		Colors::Black, Color(80, 80, 80, 255));
	render::text(globals::menuX + m_X + 3 - textSize.x, globals::menuY + m_Y + 2, fonts::menuFont,
		isGettingKey ? "..." : name, false, isGettingKey ? Colors::LightBlue : Color(180, 180, 180, 255));
}

bool GUI::KeyHolder::isActive() const
{
	return m_active;
}