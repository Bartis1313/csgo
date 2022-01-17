#include "gui.hpp"

GUI::Slider::Slider(const std::string& title, const int x, const int y, const int width, const int height,
	const Color& color, const Color& secColor, std::pair<float, float> minMax, float* value)
	: m_title{ title }, m_color{ color }, m_secColor{ secColor }, m_minMax{ minMax }, m_value{ value }
{
	setPos(x, y, width, height);
}

void GUI::Slider::draw()
{
	bool isInRange = isMouseInRange(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height);
	// needed for further calculations
	const float deltaBetween = m_minMax.second - m_minMax.first;

	// some floating point

	auto copy = *m_value;

	std::string toDraw = std::format("{} - {:.2f}", m_title, *m_value);

	render::text(globals::menuX + m_X, globals::menuY + m_Y - 10, fonts::menuFont, toDraw, false, Colors::White);

	// run master active switch
	static bool toChange = false;
	static auto check = [this]() -> void
	{
		m_active = !m_active;
		toChange = false;
	};
	// willl need to somewhat detect it better
	if (!globals::grabbingWindow)
	{
		if (isKeyPressed(VK_LBUTTON) && isInRange)
		{
			if (toChange)
				check();
		}
		else if (isInRange)
		{
			toChange = true;
		}

		// if we are in range and pressed, then change slider with current delta
		if (isKeyDown(VK_LBUTTON) && this->isActive())
		{
			*m_value = m_minMax.first + deltaBetween * (globals::mouseX - (globals::menuX + m_X)) / m_width;
		}
		else if (!isKeyDown(VK_LBUTTON) && !isInRange && this->isActive())
		{
			m_active = false;
		}
	}

	// do not go out of ranges
	*m_value = std::clamp(*m_value, m_minMax.first, m_minMax.second);

	// how many pixels to move, or call it offset.
	const float padX = ((*m_value - m_minMax.first) / deltaBetween) * m_width;

	render::drawFilledRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, m_secColor);
	render::drawGradient(globals::menuX + m_X + 1, globals::menuY + m_Y + 1, padX - 2, m_height - 2, m_color, m_secColor, false);

	if (this->isActive())
		render::drawFilledRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Color(90, 90, 90, 125));

	render::drawOutlineRect(globals::menuX + m_X, globals::menuY + m_Y, m_width, m_height, Colors::Black);
}

bool GUI::Slider::isActive() const
{
	return m_active;
}