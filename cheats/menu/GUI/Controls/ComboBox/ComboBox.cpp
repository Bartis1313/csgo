#include "ComboBox.hpp"

GUI::ComboBox::ComboBox(const std::string& title, const std::vector<std::string>& names, int* option)
	: m_title{ title }, m_names{ names }, m_option{ option }
{
	setElement(COMBOBOX_WIDTH, COMBOBOX_HEIGHT);
	setPadding(m_height + 22);
	m_type = COMBOBOX;
}

void GUI::ComboBox::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	if (m_names.empty())
		return;

	// range of first window, showing what feature is represented
	bool isInRange = isMouseInRange(pos->x, pos->y + 15, m_width, m_height);

	// text on up with padding
	render::text(pos->x + PAD_TEXT_X, pos->y, fonts::menuFont, m_title, false, this->isActive() ? Colors::White : Color(200, 220, 200, 200));

	render::drawGradient(pos->x + PAD_TEXT_X, pos->y + 15, m_width, m_height, Color(30, 30, 30, 255),
		(isInRange || this->isActive()) ? Color(60, 60, 60, 255) : Color(40, 40, 40, 255), false);

	render::drawOutlineRect(pos->x + PAD_TEXT_X, pos->y + 15, m_width, m_height, Colors::Black);

	// when active, change box to be not active
	auto check = [this]() -> void
	{
		m_active = !m_active;
		m_toChange = false;
	};

	// whenever we click the main box to show list of features
	bool toSkip = false;

	if (isKeyPressed(VK_LBUTTON) && isInRange)
	{
		if (m_toChange)
			check();
		toSkip = true;
	}
	else if (isInRange)
	{
		m_toChange = true;
	}

	// displays ... when the feature is empty
	render::text(pos->x + PAD_TEXT_X + (m_width / 2), pos->y + 15 + 3, fonts::menuFont, this->isOptionOn() ? m_names.at(*m_option) : XOR("..."),
		true, this->isActive() ? Colors::LightBlue : Color(200, 220, 200, 200));


	if (this->isActive())
	{
		if (!skipCall)
		{
			parent->skip(this, pos);
		}

		// draw background for features
		render::drawFilledRect(pos->x + PAD_TEXT_X, pos->y + 15 + m_height, m_width, (m_names.size() * m_height), Color(40, 40, 40, 255));

		for (int i = 0; i < m_names.size(); i++)
		{
			int x = pos->x + PAD_TEXT_X;
			int y = pos->y + 15 + m_height + (i * m_height);

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
				render::drawFilledRect(x, y, m_width, m_height, Color(30, 30, 30, 255));
			}

			render::text(x + (m_width / 2), y + 3, fonts::menuFont, m_names.at(i), true,
				*m_option == i ? Colors::LightBlue : isInSingleBox ? Color(200, 200, 200, 255) : Color(150, 150, 150, 255));

			if (i != 0)
			{
				render::drawLine(x + 5, y, x + m_width - 5, y, i % 2 ? Color(220, 110, 150, 70) : Color(0, 140, 250, 70));
			}
		}

		bool isInBoxRange = isMouseInRange(pos->x + PAD_TEXT_X, pos->y + m_height + 15, m_width, (m_names.size() * m_height));

		if (isKeyPressed(VK_LBUTTON) && !toSkip && !isInBoxRange)
		{
			m_active = false;
		}

		render::drawOutlineRect(pos->x + PAD_TEXT_X, pos->y + m_height - 1 + 15, m_width, (m_names.size() * m_height) + 1, Colors::Black);
	}

	pos->y += this->getPadding();
}

bool GUI::ComboBox::isActive()
{
	return m_active;
}

bool GUI::ComboBox::isOptionOn() const
{
	return *m_option != -1;
}