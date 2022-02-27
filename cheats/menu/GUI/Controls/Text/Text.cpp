#include "Text.hpp"

GUI::Text::Text(const std::string& title)
	: m_title{ title }
{
	setElement(15, 15); // does not matter here
	setPadding(18);
	m_type = TEXT;
}

void GUI::Text::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	render.text(pos->x + PAD_TEXT_X, pos->y - 1, fonts::menuFont, m_title, false, Color(200, 220, 200, 200));

	pos->y += this->getPadding();
}
