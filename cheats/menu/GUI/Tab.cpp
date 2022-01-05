#include "gui.hpp"

GUI::Tab::Tab(const std::string& title, const int x, const int y, const int width, const int height, unsigned long font, const Color& color)
	: m_title{ title }, m_font{ font }, m_color{color}
{
	setPos(x, y, width, height);
}

void GUI::Tab::draw()
{
	render::drawFilledRect(m_X, m_Y, m_width, m_height, Color(80, 80, 80, 255));

	render::text(m_X, m_Y, m_font, m_title, true, m_color);
}