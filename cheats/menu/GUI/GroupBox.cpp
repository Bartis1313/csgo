#include "gui.hpp"

GUI::GroupBox::GroupBox(const std::string& title, const int x, const int y, const int width, const int height,
	const std::vector<std::string>& names, const Color& color, const Color& secColor, int* option)
	: m_title{ title }, m_names{ names }, m_color{ color }, m_secColor{ secColor }, m_option{ option }
{
	setPos(x, y, width, height);
}

void GUI::GroupBox::draw()
{
	if (m_names.empty())
		return;

	// range of first window, showing what feature is represented
	bool isInRange = isMouseInRange(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height);

	// text on up with padding
	render::text(globals::menuX + m_X, globals::menuY + m_Y - 15, fonts::menuFont, m_title, false, Colors::White);

	render::drawGradient(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, m_color,
		(isInRange || this->isActive()) ? Color(m_color.r() + 30, m_color.g() + 30, m_color.b() + 30, m_color.a()) : m_secColor, false);

	render::drawOutlineRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Colors::Black);

	// when active, change box to be not active
	static bool toChange = false;
	static auto check = [this]() -> void
	{
		m_active = !m_active;
		toChange = false;
	};

	// whenever we click the main box to show list of features
	bool toSkip = false;

	if (isKeyPressed(VK_LBUTTON) && isInRange)
	{
		if (toChange)
			check();
		toSkip = true;
	}
	else if (isInRange)
	{
		toChange = true;
	}

	// displays ... when the feature is empty
	render::text(globals::menuX + m_X + (m_width / 2), globals::menuY + m_Y + 3, fonts::menuFont, this->isOptionOn() ? m_names.at(*m_option) : "...",
		true, this->isActive() ? Colors::LightBlue : Colors::White);

	if (this->isActive())
	{
		// draw background for features
		render::drawFilledRect(globals::menuX + m_X, globals::menuY + m_Y + m_height, m_width, (m_names.size() * m_height), m_secColor);

		for (int i = 0; i < m_names.size(); i++)
		{
			int x = globals::menuX + m_X;
			int y = globals::menuY + m_Y + m_height + (i * m_height);

			// if single rectangle representing feature was hovered
			bool isInSingleBox = isMouseInRange(x, y, m_width, m_height);

			if (isKeyPressed(VK_LBUTTON) && isInSingleBox)
			{
				// gets OLD value, needed to make a nice addon
				// addon as: remove to -1 if we clicked on same again
				int old = *m_option;
				*m_option = i;

				if (old == *m_option)
				{
					*m_option = -1;
				}
			}

			// draw shadow on hovered feature
			if (isInSingleBox)
			{
				render::drawFilledRect(x, y, m_width, m_height, m_color);
			}

			render::text(x + (m_width / 2), y + 3, fonts::menuFont, m_names.at(i), true,
				*m_option == i ? Colors::LightBlue : isInSingleBox ? Color(200, 200, 200, 255) : Color(150, 150, 150, 255));

			if (i != 0)
			{
				render::drawLine(x + 5, y, x + m_width - 5, y, i % 2 ? Color(220, 110, 150, 70) : Color(0, 140, 250, 70));
			}
		}

		if (isKeyPressed(VK_LBUTTON) && !toSkip)
		{
			m_active = false;
		}

		render::drawOutlineRect(globals::menuX + m_X, globals::menuY + m_Y + m_height - 1, m_width, (m_names.size() * m_height) + 1, Colors::Black);
	}
}

bool GUI::GroupBox::isActive() const
{
	return m_active;
}

bool GUI::GroupBox::isOptionOn() const
{
	return *m_option != -1;
}