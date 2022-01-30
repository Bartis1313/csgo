#include "Button.hpp"

GUI::Button::Button(const std::string& title, const std::function<void()>& lambda)
	: m_title{ title }, m_lamda{ lambda }
{
	setElement(BUTTON_WIDTH, BUTTON_HEIGHT);
	setPadding(25);
}

void GUI::Button::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	bool isInRange = isMouseInRange(pos->x, pos->y, m_width, m_height);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		m_lamda();

		//printf("zmeinilo\n");
	}

	render::drawGradient(pos->x, pos->y, m_width, m_height,
		Color(50, 120, 220, 255), Colors::Black, false);

	render::drawOutlineRect(pos->x + 1, pos->y + 1, m_width - 2, m_height - 2, isInRange ? Color(50, 120, 220, 255) : Color(50, 120, 180, 255));

	if (isInRange)
		render::drawFilledRect(pos->x, pos->y, m_width, m_height, Colors::Grey);

	render::drawOutlineRect(pos->x, pos->y, m_width, m_height, Colors::Black);

	// TODO: add better calculation based on font
	render::text(pos->x + (m_width / 2), pos->y + 2, fonts::menuFont, m_title, true,
		isInRange ? Color(200, 170, 70, 255) : Colors::White);

	pos->y += this->getPadding();
}