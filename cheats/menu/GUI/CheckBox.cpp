#include "gui.hpp"

GUI::CheckBox::CheckBox(const std::string& title, const int x, const int y, const int width, const int height, const Color& color, bool* feature)
	: m_title{ title }, m_color{ color }, m_feature{ feature }
{
	setPos(x, y, width, height);
}

void GUI::CheckBox::draw()
{
	bool isInRange = isMouseInRange(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		*m_feature = !*m_feature;

		//printf("zmeinilo\n");
	}

	render::drawGradient(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height,
		*m_feature ? Colors::Green : Color(70, 70, 70, 255), *m_feature ? Colors::Black : Color(70, 70, 70, 255), false);

	if (isInRange)
		render::drawFilledRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Colors::Grey);

	render::drawOutlineRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Colors::Black);
	// TODO: do changes after fonts struct is added
	render::text(globals::menuX + m_X + 20, globals::menuY + m_Y, fonts::menuFont, m_title, false, Colors::White);
}

bool GUI::CheckBox::isActive() const
{
	return *m_feature;
}