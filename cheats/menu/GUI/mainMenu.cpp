#include "mainMenu.hpp"
#include "Controls/Tab/Tab.hpp"
#include "../../globals.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

GUI::Menu::Menu(const short key)
	: m_keyActivate{ key },
	m_X{ MENU_START_X }, m_Y{ MENU_START_Y },
	m_width{ MENU_WIDTH }, m_height{ MENU_HEIGHT },
	m_offsetX{ 15 }, m_offsetY{ 30 }
{}

GUI::Tab* GUI::Menu::addTab(const std::string& name)
{
	auto t = new Tab(name);
	t->setParent(this);
	m_tabs.emplace_back(t);
	return t;
}

void GUI::Menu::skip(Element* elementToSkip, Vector2D* pos)
{
	m_elementToSkip = elementToSkip;
	m_oldPos = *pos;
	if (isKeyDown(VK_LBUTTON))
	{
		m_skipClick = true;
		raiseKey(VK_LBUTTON);
	}
}

void GUI::Menu::drawAllTabs(Vector2D* pos)
{
	if (m_tabs.empty())
		return;

	// have to somehow see what tab has been pressed
	static size_t tabNow = m_selectedTab;
	if (m_selectedTab != tabNow)
	{
		tabNow = m_selectedTab;
	}

	// based on menu size, and amount of tabs, you don't have to care about giving all values to draw in selected pos
	static int size = ((MENU_WIDTH - 6) / m_tabs.size());

	// fix style of this and pos
	for (int i = 0; i < m_tabs.size(); i++)
	{
		int x = pos->x + 4 + (i * size);
		int y = pos->y + 3;
		int w = size;
		int h = TAB_HEIGHT;

		bool isInRange = isMouseInRange(x, y, w, h);
		bool isSelected = m_selectedTab == i;

		if (isInRange && isKeyPressed(VK_LBUTTON))
		{
			m_selectedTab = i;
			selectedTabName = m_tabs.at(i)->getName();
			//printf("ostatnie %i \n", globals::lastTab);
		}

		render::drawGradient(x, y, w, h,
			isSelected ? Color(50, 120, 220, 255) : Color(50, 120, 180, 255),
			isSelected ? Colors::Black : Color(50, 120, 180, 255), false);

		render::drawOutlineRect(x + 1, y + 1, w - 2, h - 2,
			isInRange ? Color(50, 120, 220, 255) : Color(50, 120, 180, 255));

		if (isInRange)
			render::drawFilledRect(x, y, w, h, Colors::Grey);

		render::drawOutlineRect(x, y, w, h, Colors::Black);

		// TODO: add better calculation based on font
		render::text(x + (size / 2), y + 3, fonts::menuFont, m_tabs.at(i)->getName(), true, isSelected ? Color(200, 170, 70, 255) : Colors::White);
	}

	pos->x += this->getStartX();
	pos->y += this->getStartY();
}

void GUI::Menu::draw()
{
	// init first name
	if (this->getSelectedName().empty() && m_tabs.size())
	{
		this->setSelectedName(m_tabs.at(0)->getName());
	}

	if (isKeyPressed(m_keyActivate))
	{
		m_opened = !m_opened;
	}

	if (!m_opened)
		return;

	static std::pair<int, int> mousePos;
	static std::pair<int, int> menuPos;

	if (isKeyDown(VK_LBUTTON))
	{
		//printf("mouse x %i, mouse y %i\n", mousePos.first, mousePos.second);

		//printf("m_X %i, m_Y %i\n", m_X, m_Y);

		if (!m_dragging && isMouseInRange(m_X - THICKNESS, m_Y - (THICKNESS * 4), m_width - (THICKNESS * 2), THICKNESS * 4))
		{
			m_dragging = true;
			mousePos = { globals::mouseX, globals::mouseY };
			menuPos = { m_X, m_Y };
		}
		if (m_dragging)
		{
			//printf("dragging\n");

			m_X = menuPos.first + globals::mouseX - mousePos.first;
			m_Y = menuPos.second + globals::mouseY - mousePos.second;
		}
	}
	else
		m_dragging = false;

	m_X = std::clamp(m_X, 0 + static_cast<int>(THICKNESS), globals::screenX - m_width - static_cast<int>(THICKNESS));
	m_Y = std::clamp(m_Y, 0 + static_cast<int>(THICKNESS * 4), globals::screenY - m_height - static_cast<int>(THICKNESS));

	drawBackground();
	drawExtraBackgroundLines();
	// originally I thought it's better to add circle, rect looks better I guess
	drawCloseRect();

	// outlines to make cool look
	render::drawOutlineRect(m_X - 1, m_Y - 1, m_width + 1, m_height + 1, Colors::Black);
	render::drawOutlineRect(m_X - THICKNESS - 1, m_Y - (THICKNESS * 4) - 1, m_width + (THICKNESS * 2) + 1, m_height + (THICKNESS * 5) + 1, Colors::Black);

	Vector2D pos = Vector2D(m_X, m_Y);

	drawAllTabs(&pos);

	// this draws content
	for (const auto& el : m_tabs)
	{
		if (el->getName() == this->getSelectedName())
		{
			el->draw(&pos);
		}
	}

	pos.x += this->getStartX();
	pos.y += this->getStartY();

	if (m_elementToSkip)
	{
		if (m_skipClick)
		{
			forceKey(VK_LBUTTON);
		}
		reinterpret_cast<Element*>(m_elementToSkip)->draw(&m_oldPos, this, true);
		m_skipClick = false;
		m_elementToSkip = nullptr;
	}
}

void GUI::Menu::drawBackground() const
{
	render::drawFilledRect(m_X, m_Y, m_width, m_height, Color(50, 50, 50, 255));
}

void GUI::Menu::drawExtraBackgroundLines() const
{
	// left
	render::drawGradient(m_X - THICKNESS, m_Y, THICKNESS, m_height + THICKNESS, Colors::LightBlue, Colors::Palevioletred, false);
	// right
	render::drawGradient(m_X + m_width, m_Y, THICKNESS, m_height + THICKNESS, Colors::Palevioletred, Colors::LightBlue, false);
	// bottom
	render::drawGradient(m_X, m_Y + m_height, m_width, THICKNESS, Colors::Palevioletred, Colors::LightBlue, true);

	// top - header
	render::drawGradient(m_X - THICKNESS, m_Y - (THICKNESS * 4), m_width + (THICKNESS * 2), THICKNESS * 4, Colors::LightBlue, Colors::Palevioletred, true);
	render::text(m_X, m_Y - (THICKNESS * 3) - 2, fonts::tahoma, XOR("CSGO legit"), false, Colors::Palevioletred);
}

void GUI::Menu::drawCloseRect()
{
	static int size = THICKNESS * 4;
	int x = m_X + m_width - size + THICKNESS;
	int y = m_Y - size;

	bool isInRange = isMouseInRange(x, y, size, size);

	if (isInRange && isKeyPressed(VK_LBUTTON))
	{
		m_opened = !m_opened;
	}

	render::drawFilledRect(x, y, size, size, isInRange ? Colors::Red : Color(128, 128, 128, isInRange ? 255 : 150));
	
	render::drawLine(x, y, x + size, y + size, isInRange ? Colors::Black : Color(40, 40, 40, 150));
	render::drawLine(x, y, x + size, y + size, isInRange ? Colors::Black : Color(40, 40, 40, 150));
	render::drawLine(x, y + size, x + size, y, isInRange ? Colors::Black : Color(40, 40, 40, 150));
	render::drawLine(x, y + size, x + size, y, isInRange ? Colors::Black : Color(40, 40, 40, 150));
}

void GUI::Menu::shutdown()
{
	for (const auto& el : m_tabs)
	{
		el->shutdown();
	}
}