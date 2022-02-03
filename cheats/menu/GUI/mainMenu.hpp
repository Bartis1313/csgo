#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../../../SDK/math/Vector2D.hpp"

namespace GUI
{
	class Tab;
	class Element;

	// menu class, changed from window, because menu itself is an instance
	class Menu final
	{
	public:
		Menu() = default;
		Menu(const short key);
		void draw();
		void setSelectedName(const std::string& name) { selectedTabName = name; }
		void skip(Element* elementToSkip, Vector2D* pos);
		void setSelected(const int idx) { m_selectedTab = idx; }
		void shutdown();

		_NODISCARD Tab* addTab(const std::string& name);

		_NODISCARD bool isOpened() const { return m_opened; }
		_NODISCARD bool isDragging() const { return m_dragging; }
		_NODISCARD int getX() const { return m_X; }
		_NODISCARD int getY() const { return m_Y; }
		_NODISCARD int getWidth() const { return m_width; }
		_NODISCARD int getHeight() const { return m_height; }
		_NODISCARD int getSelected() const { return m_selectedTab; }
		_NODISCARD std::string getSelectedName() const { return selectedTabName; }
		_NODISCARD int getMenuDrawHeight() const { return m_Y + m_offsetY; }
		_NODISCARD int getStartX() const { return m_offsetX; }
		_NODISCARD int getStartY() const { return m_offsetY; }
		_NODISCARD size_t getThickness() const { return THICKNESS; }
		_NODISCARD std::vector<Tab*> getTab() const { return m_tabs; }
	private:
		void drawBackground() const;
		void drawExtraBackgroundLines() const;
		void drawAllTabs(Vector2D* pos);
		void drawCloseRect();

		int m_X, m_Y;
		int m_width, m_height;
		bool m_opened = false;
		bool m_dragging;
		std::vector<Tab*> m_tabs;
		int m_selectedTab = 0;
		std::string selectedTabName = "";
		size_t THICKNESS = 5;
		short m_keyActivate;
		Element* m_elementToSkip = nullptr;
		bool m_skipClick;
		Vector2D m_oldPos;
		int m_offsetX;
		int m_offsetY;
	};
}