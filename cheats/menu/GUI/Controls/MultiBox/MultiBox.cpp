#include "MultiBox.hpp"

GUI::MultiBox::MultiBox(const std::string& title, const std::vector<std::string>& names, std::vector<bool>* feature)
	: m_title{ title }, m_names{ names }, m_listedOptions{ feature }
{
	setElement(MULTIBOX_WIDTH, MULTIBOX_HEIGHT);
}

void GUI::MultiBox::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	if (m_names.empty())
		return;

	// range of first window, showing what feature is represented
	bool isInRange = isMouseInRange(pos->x, pos->y, m_width, m_height);

	// text on up with padding
	render::text(pos->x, pos->y - 15, fonts::menuFont, m_title, false, Colors::White);

	render::drawGradient(pos->x, pos->y, m_width, m_height, Color(30, 30, 30, 255),
		(isInRange || this->isActive()) ? Color(60, 60, 60, 255) : Color(40, 40, 40, 255), false);

	render::drawOutlineRect(pos->x, pos->y, m_width, m_height, Colors::Black);

	// copy of argument
	std::vector<bool> options = *m_listedOptions;

	// fill the string with correct form
	std::string allText;
	for (int i = 0; i < m_names.size(); i++)
	{
		if (options.at(i))
		{
			if (allText.length())
			{
				allText += " ";
			}
			allText += m_names.at(i);
			if (auto size = render::getTextSize(fonts::menuFont, allText); size > m_width - 4)
			{
				// hardcoded
				allText = allText.substr(0, 30).append("...");
			}
		}
	}

	render::text(pos->x + (m_width / 2), pos->y + 3, fonts::menuFont, !allText.empty() ? allText : "...", true,
		this->isActive() ? Colors::LightBlue : Colors::White);

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

	// checks for multibox choices, this one needs same way of handling main box of activity
	static bool toChangeSingle = false;

	static auto checkSingle = [&](const size_t idx) -> void
	{
		options.at(idx) = !options.at(idx);
		toChangeSingle = false;
	};

	if (this->isActive())
	{
		if (!skipCall)
		{
			parent->skip(this, pos);
		}

		// draw background for features
		render::drawFilledRect(pos->x, pos->y + m_height, m_width, (m_names.size() * m_height), Color(40, 40, 40, 255));

		for (int i = 0; i < m_names.size(); i++)
		{
			int x = pos->x;
			int y = pos->y + m_height + (i * m_height);

			// if single rectangle representing feature was hovered
			bool isInSingleBox = isMouseInRange(x, y, m_width, m_height);

			if (isKeyPressed(VK_LBUTTON) && isInSingleBox)
			{
				if (toChangeSingle)
					checkSingle(i);
			}
			else if (isInSingleBox)
			{
				toChangeSingle = true;
			}

			// draw shadow on hovered feature
			if (isInSingleBox)
			{
				render::drawFilledRect(x, y, m_width, m_height, Color(30, 30, 30, 255));
			}

			render::text(x + (m_width / 2), y + 3, fonts::menuFont, m_names.at(i), true,
				options.at(i) ? Colors::LightBlue : isInSingleBox ? Color(200, 200, 200, 255) : Color(150, 150, 150, 255));

			if (i != 0)
			{
				render::drawLine(x + 5, y, x + m_width - 5, y, i % 2 ? Color(220, 110, 150, 70) : Color(0, 140, 250, 70));
			}
		}

		*m_listedOptions = options;

		if (isKeyPressed(VK_LBUTTON) && !toSkip)
		{
			m_active = false;
		}

		render::drawOutlineRect(pos->x, pos->y + m_height - 1, m_width, (m_names.size() * m_height) + 1, Colors::Black);
	}

	pos->y += 40;
}

bool GUI::MultiBox::isActive() const
{
	return m_active;
}

bool GUI::MultiBox::isOptionOn(const size_t idx) const
{
	return m_listedOptions->at(idx) != -1;
}