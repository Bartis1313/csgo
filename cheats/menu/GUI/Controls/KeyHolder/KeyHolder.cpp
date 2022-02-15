#include "KeyHolder.hpp"
#include "../../../../../utilities/utilities.hpp"

GUI::KeyHolder::KeyHolder(int* key)
	: m_key{ key }
{
	setElement(KEYHOLDER_WIDTH, KEYHOLDER_HEIGHT);
	setPadding(0);
	m_type = KEY_HOLDER;
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

void GUI::KeyHolder::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	// only on start
	std::string name = XOR("init");

	// if key is given, then change name to "..." waiting state
	if (m_isGettingKey)
	{
		name = XOR("...");

		// looop through possible choices
		for (short i = 1; i < 256; i++)
		{
			if (isKeyPressed(i))
			{
				// is bad key? then throw some unknown index
				isbadKey(i) ? *m_key = -1 : *m_key = i;
				m_isGettingKey = false;
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

	// long names can be problematic, or numpad
	if (name.length() > 5 || name.starts_with(XOR("Num")))
	{
		auto temp = utilities::splitStr(name);
		name.clear();
		for (const auto& el : temp)
		{
			name += el[0];
		}
	}

	// to align better? yes
	Vector2D textSize = render::getTextSizeXY(fonts::menuFont, name);

	bool isInRange = isMouseInRange(pos->x + PAD_TEXT_X + BOX_WIDTH + 5, pos->y - 20, textSize.x + 5, textSize.y + 3);

	if (isKeyPressed(VK_LBUTTON) && isInRange)
	{
		m_isGettingKey = true;
	}

	// isGettingKey can be this->isActive() in this case, soon it will be patched with extras

	render::drawFilledRect(pos->x + PAD_TEXT_X + BOX_WIDTH + 5, pos->y - 20, textSize.x + 5, textSize.y + 3, Color(40, 40, 40, 255));
	render::drawOutlineRect(pos->x + PAD_TEXT_X + BOX_WIDTH + 5, pos->y -20, textSize.x + 5, textSize.y + 3, Colors::Black);
	render::text(pos->x + PAD_TEXT_X + BOX_WIDTH + 8, pos->y - 18, fonts::menuFont,
		m_isGettingKey ? XOR("...") : name, false, m_isGettingKey ? Colors::LightBlue : Color(180, 180, 180, 255));
}

bool GUI::KeyHolder::isActive()
{
	return m_active;
}