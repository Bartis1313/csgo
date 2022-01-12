#include "gui.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

GUI::Window::Window(const int x, const int y, const int width, const int height)
{
	setPos(x, y, width, height);
}

void GUI::Window::initWindow()
{
	static bool isDragging = false;
	static std::pair<int, int> mousePos;
	static std::pair<int, int> menuPos;

	if (isKeyDown(VK_LBUTTON))
	{
		//printf("mouse x %i, mouse y %i\n", globals::mouseX, globals::mouseY);

		if (!isDragging && isMouseInRange(globals::menuX, globals::menuY, globals::menuWidth, globals::menuHeight))
		{
			isDragging = true;
			mousePos = { globals::mouseX, globals::mouseY };
			menuPos = { globals::menuX, globals::menuY };
		}
		if (isDragging)
		{
			//printf("dragging\n");

			globals::menuX = menuPos.first + globals::mouseX - mousePos.first;
			globals::menuY = menuPos.second + globals::mouseY - mousePos.second;
		}
	}
	else
		isDragging = false;

	static int x, y;
	interfaces::engine->getScreenSize(x, y);
	globals::menuX = std::clamp(globals::menuX, 0 + static_cast<int>(THICKNESS), x - globals::menuWidth - static_cast<int>(THICKNESS));
	globals::menuY = std::clamp(globals::menuY, 0 + static_cast<int>(THICKNESS), y - globals::menuHeight - static_cast<int>(THICKNESS));

	m_X = globals::menuX;
	m_Y = globals::menuY;
}


std::array<int, 4> GUI::Window::getWindowPos() const
{
	return { m_X, m_Y, m_width, m_height };
}

void GUI::Window::draw()
{
	drawBackground();
	drawExtraBackgroundLines();
}

void GUI::Window::drawBackground() const
{
	render::drawFilledRect(globals::menuX, globals::menuY, m_width, m_height, Color(50, 50, 50, 255));
}

void GUI::Window::drawExtraBackgroundLines() const
{
	// left
	render::drawGradient(globals::menuX - THICKNESS, globals::menuY - THICKNESS, THICKNESS, m_height + (THICKNESS * 2), Colors::LightBlue, Colors::Palevioletred, false);
	// top 
	render::drawGradient(globals::menuX, globals::menuY - THICKNESS, m_width, THICKNESS, Colors::LightBlue, Colors::Palevioletred, true);
	// right
	render::drawGradient(globals::menuX + m_width, globals::menuY - THICKNESS, THICKNESS, m_height + (THICKNESS * 2), Colors::Palevioletred, Colors::LightBlue, false);
	// bottom
	render::drawGradient(globals::menuX, globals::menuY + m_height, m_width, THICKNESS, Colors::Palevioletred, Colors::LightBlue, true);
}