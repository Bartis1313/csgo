#include "CheckBox.hpp"

GUI::CheckBox::CheckBox(const std::string& title, bool* feature)
	: m_title{ title }, m_feature{ feature }
{
	setElement(CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
	setPadding(18);
	m_type = CHECKBOX;
}

void GUI::CheckBox::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	bool isInRange = isMouseInRange(pos->x, pos->y, m_width, m_height);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		*m_feature = !*m_feature;

		//printf("zmeinilo\n");
	}

	render::drawGradient(pos->x, pos->y, m_width, m_height,
		this->isActive() ? Colors::LightBlue : Color(70, 70, 70, 255), this->isActive() ? Colors::Black : Color(70, 70, 70, 255), false);

	if (isInRange)
		render::drawFilledRect(pos->x, pos->y, m_width, m_height, Colors::Grey);

	render::drawOutlineRect(pos->x, pos->y, m_width, m_height, Colors::Black);
	// TODO: do changes after fonts struct is added
	render::text(pos->x + PAD_TEXT_X, pos->y - 1, fonts::menuFont, m_title, false, this->isActive() ? Colors::White : Color(200, 220, 200, 200));

	pos->y += this->getPadding();
}

bool GUI::CheckBox::isActive()
{
	return *m_feature;
}