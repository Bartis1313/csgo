#include "gui.hpp"

GUI::Tab::Tab(const std::string& title, const int x, const int y, const int width, const int height, const Color& color)
	: m_title{ title }, m_color{color}
{
	setPos(x, y, width, height);
}

void GUI::Tab::draw()
{
	bool isInRange = isMouseInRange(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		m_active = !m_active;

		//printf("zmeinilo\n");
	}

	// prob shouldn't be rounded for case of looking better

	render::drawRoundedRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, 10, 16,
		m_active ? Color(50, 120, 220, 255) : Color(50, 120, 180, 255));

	if (isInRange)
		render::drawRoundedRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, 10, 16, Color(50, 100, 140, 255));

	render::text(globals::menuX + m_X + 3, globals::menuY + m_Y, fonts::menuFont, m_title, false, Colors::White);
}

bool GUI::Tab::isActive() const
{
	return m_active;
}