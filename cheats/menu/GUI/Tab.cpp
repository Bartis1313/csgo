#include "gui.hpp"

GUI::Tab::Tab(const std::string& title)
	: m_title{ title }
{}

GUI::Tab::Tab(const std::span<Tab>& arr, const size_t height, const size_t paddingXbetween,
	const size_t paddingY, const Color& color, const Color& secColor)
	: m_array{ arr }, m_paddingBetween{ paddingXbetween }, m_paddingY{ paddingY },
	m_color{ color }, m_secColor{ secColor }
{
	m_height = height;
}

void GUI::Tab::draw()
{
	if (m_array.empty())
		return;

	// have to somehow see what tab has been pressed
	static size_t tabNow = m_selectedTab;
	if (m_selectedTab != tabNow)
	{
		tabNow = m_selectedTab;
	}

	// based on menu size, and amount of tabs, you don't have to care about giving all values to draw in selected pos
	static int size = (globals::menuWidth / m_array.size()) - m_paddingBetween;

	for (int i = 0; i < m_array.size(); i++)
	{
		int x = globals::menuX + m_paddingBetween + (i * size);
		int y = globals::menuY + m_paddingY;
		int w = size;
		int h = m_height;

		bool isInRange = isMouseInRange(x, y, w, h);
		bool isSelected = m_selectedTab == i;

		if (isInRange && isKeyPressed(VK_LBUTTON))
		{
			m_selectedTab = i;

			//printf("ostatnie %i \n", globals::lastTab);
		}

		render::drawGradient(x, y, w, h,
			isSelected ? m_color : m_secColor,
			isSelected ? Colors::Black : m_secColor, false);

		render::drawOutlineRect(x + 1, y + 1, w - 2, h - 2,
			isInRange ? m_color : m_secColor);

		if (isInRange)
			render::drawFilledRect(x, y, w, h, Colors::Grey);

		render::drawOutlineRect(x, y, w, h, Colors::Black);

		// TODO: add better calculation based on font
		render::text(x + (size / 2), y + 3, fonts::menuFont, m_array[i].m_title, true, isSelected ? Color(200, 170, 70, 255) : Colors::White);
	}
}

size_t GUI::Tab::getSelected() const
{
	return m_selectedTab;
}