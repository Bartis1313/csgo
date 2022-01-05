#include "gui.hpp"

template<typename T>
GUI::Button<T>::Button(const std::string& title, const int x, const int y, const int width, const int height, unsigned long font, const Color& color, T* feature)
	: m_title{ title }, m_font{ font }, m_color{color}, m_feature{ feature }
{
	setPos(x, y, width, height);
}

template<typename T>
bool GUI::Button<T>::isClicked()
{
	if (isKeyPressed(VK_LBUTTON) && isMouseInRange(m_X, m_Y, m_width, m_height))
	{
		return true;
	}
	return false;
}

template<typename T>
void GUI::Button<T>::draw()
{
	render::drawFilledRect(m_X, m_Y, m_width, m_height, Color(80, 80, 80, 255));

	render::text(m_X, m_Y, m_font, m_title, true, m_color);
}