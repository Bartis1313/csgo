#include "gui.hpp"

GUI::Button::Button(const std::string& title, const int x, const int y, const int width, const int height, const Color& color)
	: m_title{ title }, m_color{ color }
{
	setPos(x, y, width, height);
}

bool GUI::Button::isClicked()
{
	if (isKeyDown(VK_LBUTTON) && isMouseInRange(m_X, m_Y, m_width, m_height))
	{
		return true;
	}
	return false;
}

void GUI::Button::draw()
{
	render::drawFilledRect(m_X, m_Y, m_width, m_height, Color(80, 80, 80, 255));

	render::text(m_X, m_Y, fonts::menuFont, m_title, true, m_color);
}