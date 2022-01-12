#include "gui.hpp"

GUI::Button::Button(const std::string& title, const int x, const int y, const int width, const int height,
	const Color& color, const Color& secColor, const std::function<void()>& lambda)
	: m_title{ title }, m_color{ color }, m_secColor{ secColor }, m_lamda{ lambda }
{
	setPos(x, y, width, height);
}

void GUI::Button::draw()
{
	bool isInRange = isMouseInRange(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		m_lamda();

		//printf("zmeinilo\n");
	}
	
	render::drawGradient(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height,
		m_color, Colors::Black, false);

	render::drawOutlineRect(globals::menuX + m_X + 1, globals::menuY + m_Y + 1, m_width - 2, m_height - 2, isInRange ? m_color : m_secColor);

	if (isInRange)
		render::drawFilledRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Colors::Grey);

	render::drawOutlineRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Colors::Black);

	// TODO: add better calculation based on font
	render::text(globals::menuX + m_X + (m_width / 2), globals::menuY + m_Y + 2, fonts::menuFont, m_title, true,
		isInRange ? Color(200, 170, 70, 255) : Colors::White);
}