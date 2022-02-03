#include "HelpMarker.hpp"

GUI::HelpMarker::HelpMarker(const std::string& text)
	: m_text{ text }
{
	setPadding(0);
	m_type = HELP_MARKER;
}

void GUI::HelpMarker::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	if (m_text.size() > 50) // some bit hardcoded check to don't draw long text
		return;

	// size for the "(?)"
	auto size = render::getTextSizeXY(fonts::menuFont, m_helpString);

	render::text(pos->x + PAD_TEXT_X + BOX_WIDTH + 8, pos->y - (size.y / 2) + 4, fonts::menuFont, m_helpString, false, Colors::Coral);

	// rest, checks if hovered
	if (auto textVec = render::getTextSizeXY(fonts::menuFont, m_text); isMouseInRange(pos->x + PAD_TEXT_X + BOX_WIDTH + 8, pos->y - (size.y / 2) + 4, size.x, size.y))
	{
		if (!skipCall)
		{
			parent->skip(this, pos);
		}

		int x = pos->x + 170;
		int y = pos->y + 20;

		render::drawFilledRect(x, y, textVec.x + 6, textVec.y + 6, Color(128, 128, 128, 255));
		render::drawOutlineRect(x - 1, y - 1, textVec.x + 6 + 1, textVec.y + 6 + 1, Colors::Black);
		render::text(x + 3, y - 6 + (textVec.y / 2), fonts::menuFont, m_text, false, Colors::Coral);
	}
}