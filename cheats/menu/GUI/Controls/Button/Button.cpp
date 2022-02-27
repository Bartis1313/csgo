#include "Button.hpp"

GUI::Button::Button(const std::string& title, const std::function<void()>& lambda)
	: m_title{ title }, m_lamda{ lambda }
{
	setElement(BUTTON_WIDTH, BUTTON_HEIGHT);
	setPadding(m_height + 5);
	m_type = BUTTON;
}

void GUI::Button::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	bool isInRange = isMouseInRange(pos->x + PAD_TEXT_X, pos->y, m_width, m_height);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		m_lamda();
	}

	render.drawGradient(pos->x + PAD_TEXT_X, pos->y, m_width, m_height, Color(30, 30, 30, 255),
		isInRange ? Color(60, 60, 60, 255) : Color(40, 40, 40, 255), false);

	render.drawOutlineRect(pos->x + PAD_TEXT_X, pos->y, m_width, m_height, Colors::Black);

	render.text(pos->x + PAD_TEXT_X + (m_width / 2), pos->y + 2, fonts::menuFont, m_title, true,
		isInRange ? Colors::White : Color(200, 220, 200, 200));

	pos->y += this->getPadding();
}

bool GUI::Button::isActive()
{
	return m_active;
}