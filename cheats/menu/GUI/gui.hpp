#pragma once
#include <Windows.h>
#include <array>
#include <string>
#include "../../../utilities/renderer/renderer.hpp"
#include "TextureHolder.hpp"

/*
* Window -> init with actual literal pos
* other classes, init with menu globals, say you wanna get a checkbox, then init with globals::menuX + 20 and etc, etc, etc, etc
*/

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
		std::array<int, 4> getWindowPos() const;
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
		Tab(const std::string& title, const int x, const int y, const int width, const int height, const Color& color);
		bool isActive() const;
		virtual void draw() override;
	private:
		bool m_active;
		std::string m_title;
		Color m_color;
	};
	// checkbox, represents a button to change some boolean
	class CheckBox : public Element
	{
	public:
		CheckBox(const std::string& title, const int x, const int y, const int width, const int height, const Color& color, bool* feature);
		bool isActive() const;
		virtual void draw() override;
	private:
		bool m_active;
		bool* m_feature;
		std::string m_title;
		Color m_color;
	};
	// button, it has the option to turn off/on the feature
	class Button : public Element
	{
	public:
		Button(const std::string& title, const int x, const int y, const int width, const int height, const Color& color);
		bool isClicked();
		virtual void draw() override;
	private:
		std::string m_title;
		bool m_clicked;
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
	bool isKeyDown(const short key);
	bool isKeyPressed(const short key);

	// some objects
	inline bool debil = true;
	inline Window mainWindow{ globals::menuX, globals::menuY, globals::menuWidth, globals::menuHeight };
	inline CheckBox boxik{ "Test", 20, 30, 15, 15, Colors::DarkBlue, &debil };
	inline std::array<Tab, 2> windowTabs =
	{
		Tab{"Something", 10, 10, 50, 15, Colors::White},
		Tab{"Something", 60, 10, 50, 15, Colors::White}
	};

	namespace renderGUI
	{
		void test();
	}

	enum HelperPos
	{
		M_X,
		M_Y,
		M_WIDTH,
		M_HEIGHT
	};
}

namespace test
{
	void run();
	void init();
	inline TextureHolder fatchad;
}