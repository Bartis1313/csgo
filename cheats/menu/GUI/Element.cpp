#include "gui.hpp"
#include "../../globals.hpp"

bool GUI::Element::isMouseInRange(const int x, const int y, const int width, const int height)
{
	return globals::mouseX > x && globals::mouseY > y
		&& globals::mouseX < width + x && globals::mouseY < height + y;
}

void GUI::Element::setPos(const int x, const int y, const int width, const int height)
{
	m_X = x;
	m_Y = y;
	m_width = width;
	m_height = height;
}