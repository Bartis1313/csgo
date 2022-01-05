#pragma once
#include <Windows.h>
#include <utility>
#include <string>
#include "../../../utilities/renderer/renderer.hpp"
#include "TextureHolder.hpp"

namespace GUI
{
	// base class for menu gui
	class Element
	{
	public:
		bool isMouseInRange(const int x, const int y, const int width, const int height) const;
		void setPos(const int x, const int y, const int width, const int height);
	protected:
		int m_X, m_Y;
		int m_width, m_height;
		// main drawing
		virtual void draw() = 0;
	};
	// class to draw basic window to represent gui. simple rect
	class Window : public Element
	{
	public:
		Window(const int x, const int y, const int width, const int height);
		void initWindow();
		std::pair<int, int> getWindowPos() const;
		virtual void draw() override;
	private:
		void drawBackground() const;
		void drawExtraBackgroundLines() const;
		size_t THICKNESS = 5;
	};
	// class to add tabs, it's like a button, but some things are different
	class Tab : public Element
	{
	public:
		Tab(const std::string& title, const int x, const int y, const int width, const int height, unsigned long font, const Color& color);
		bool isActive();
		virtual void draw() override;
	private:
		bool m_active;
		std::string m_title;
		unsigned long m_font;
		Color m_color;
	};
	// button, it has the option to turn off/on the feature
	template<typename T>
	class Button : public Element
	{
	public:
		Button(const std::string& title, const int x, const int y, const int width, const int height, unsigned long font, const Color& color, T* feature);
		bool isClicked();
		virtual void draw() override;
	private:
		std::string m_title;
		bool m_clicked;
		T* m_feature;
		unsigned long m_font;
		Color m_color;
	};
	// button but as a list with options
	class GroupBox : public Element
	{
	public:
		virtual void draw() override;
	};
	// input that can go for numbers or letters
	class Input : public Element
	{
	public:
		virtual void draw() override;
	};
	// a very basic slider that represent some percentage or number to dynamically choice in range between
	class Slider : public Element
	{
	public:
		virtual void draw() override;
	};
	// the special case for this GUI, color picker will be based on image, since it will look nicer and easier to make
	class ColorPicker : public Element
	{
	public:
		virtual void draw() override;
	};

	namespace globals
	{
		inline int mouseX;
		inline int mouseY;

		inline int menuX = 600;
		inline int menuY = 500;
		inline int menuWidth = 800;
		inline int menuHeight = 400;

		inline short keyState[256];
		inline short previousKeyState[256];
	}
	bool isKeyPressed(const short key);

	// some objects

	inline Window mainWindow {globals::menuX, globals::menuY, globals::menuWidth, globals::menuHeight};

	namespace renderGUI
	{
		void test();
	}
}

namespace test
{
	void run();
	void init();
	inline TextureHolder fatchad;
}