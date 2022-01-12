#pragma once
#include <Windows.h>
#include <array>
#include <string>
#include <span>
#include <functional>
#include <vector>
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "TextureHolder.hpp"

// TODO: recode it a bit and make defines for few constans like: button width etc...

/*
* Window -> init with actual literal pos
* other classes, init with menu globals, say you wanna get a checkbox, then init with globals::menuX + 20 and etc, etc, etc...
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
		// init single tab, no need for X, Y pos, as this is automatically done with tab list
		Tab(const std::string& title);
		// init whole tab list
		Tab(const std::span<Tab>& arr, const size_t height, const size_t paddingXbetween,
			const size_t paddingY, const Color& color, const Color& secColor);
		size_t getSelected() const;
		virtual void draw() override;
	private:
		std::string m_title;
		Color m_color;
		Color m_secColor;
		std::span<Tab> m_array;
		size_t m_PaddingWidth;
		size_t m_paddingY;
		size_t m_paddingBetween;
		size_t m_selectedTab = 0;
	};
	// checkbox, represents a button to change some boolean
	class CheckBox : public Element
	{
	public:
		CheckBox(const std::string& title, const int x, const int y, const int width, const int height,
			const Color& color, const Color& secColor, bool* feature);
		bool isActive() const;
		virtual void draw() override;
	private:
		bool m_active;
		bool* m_feature;
		std::string m_title;
		Color m_color;
		Color m_secColor;
	};
	// button, it has the option to turn off/on the feature
	class Button : public Element
	{
	public:
		Button(const std::string& title, const int x, const int y, const int width, const int height, 
			const Color& color, const Color& secColor, const std::function<void()>& lambda);
		virtual void draw() override;
	private:
		std::string m_title;
		Color m_color;
		Color m_secColor;
		std::function<void()> m_lamda;
	};
	// button but as a list with options
	class GroupBox : public Element
	{
	public:
		// init some smaller groupbox, this one will only allow to select ONE feature
		GroupBox(const std::string& title, const int x, const int y, const int width, const int height,
			const std::vector<std::string>& names, const Color& color, const Color& secColor, int* option);
		virtual void draw() override;
		bool isActive() const;
		bool isOptionOn() const;
	private:
		std::string m_title;
		bool m_active;
		Color m_color;
		Color m_secColor;
		std::vector<std::string> m_names;
		int* m_option;
	};
	class MultiBox : public Element
	{
	public:
		// init groupbox by many features listed in array
		MultiBox(const std::string& title, const std::vector<std::string>& names, const std::vector<bool>* feature);
		virtual void draw() override;
	private:
		std::string m_title;
		Color m_color;
		Color m_secColor;
		std::vector<std::string> m_names;
		std::vector<bool>* m_listedOptions;
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

		// if pressed group/multi box, then quit adding other features at all
		// better method would be to detect if groupbox is in range of hidden features by box list
		inline bool isTakingBoxInput = false;
	}
	bool isKeyDown(const short key);
	bool isKeyPressed(const short key);

	// some objects
	inline bool somebool = true;
	inline Window mainWindow{ globals::menuX, globals::menuY, globals::menuWidth, globals::menuHeight };
	inline CheckBox somebutton{ "Test", 20, 30, 15, 15, Colors::Green, Color(70, 70, 70, 255), &somebool };
	inline std::array<Tab, 2> arrTabs =
	{
		Tab{ "Something" },
		Tab{ "Something2" }
	};
	inline Tab windowTabs{ arrTabs, 20, 8, 10, Color(50, 120, 220, 255), Color(50, 120, 180, 255) };
	inline Button button{ "Button", 10, 80, 50, 20, Color(50, 120, 220, 255), Color(50, 120, 180, 255),
		[]()
		{
			interfaces::console->consoleColorPrintf(Colors::Green, "Siema\n");
		}
	};
	inline int selected = -1;
	inline GroupBox group{ "This is a group", 20, 200, 150, 20, {"First", "Second", "Third", "Fourth", "Fith"}, Color(30, 30, 30, 255), Color(40, 40, 40, 255), &selected};

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

	enum TabNames
	{
		SOMETHING_1,
		SOMETHING_2,
	};
}

namespace test
{
	void run();
	void init();
	inline TextureHolder fatchad;
}