#pragma once
#include <Windows.h>
#include <string>
#include <array>
#include <span>
#include <utility>
#include <functional>
#include <vector>
#include <memory>
#include <any>
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
		_NODISCARD bool isMouseInRange(const int x, const int y, const int width, const int height) const;
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
		_NODISCARD std::array<int, 4> getWindowPos() const;
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
		_NODISCARD size_t getSelected() const;
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
		_NODISCARD bool isActive() const;
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
		_NODISCARD bool isActive() const;
		_NODISCARD bool isOptionOn() const;
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
		MultiBox(const std::string& title, const int x, const int y, const int width, const int height,
			const std::vector<std::string>& names, const Color& color, const Color& secColor, std::vector<bool>* feature);
		virtual void draw() override;
		_NODISCARD bool isActive() const;
		_NODISCARD bool isOptionOn(const size_t idx) const;
	private:
		std::string m_title;
		bool m_active;
		Color m_color;
		Color m_secColor;
		std::vector<std::string> m_names;
		std::vector<bool>* m_listedOptions;
	};
	// write anything to the box
	class TextInput : public Element
	{
	public:
		TextInput(const std::string& title, const int x, const int y, const int width, const int height,
			const Color& color, const Color& secColor, std::string* text);
		virtual void draw() override;
		static void initTabs();
		_NODISCARD bool isActive() const;
	private:
		static std::array<std::string, 256> m_smallLetters;
		static std::array<std::string, 256> m_bigLetters;
		static bool m_inited;
		std::string m_title;
		bool m_active;
		Color m_color;
		Color m_secColor;
		std::string* m_text;
	};
	// press anything, few keys are banned. 
	class KeyHolder : public Element
	{
	public:
		KeyHolder(const std::string& title, const int x, const int y, const int width, const int height,
			const Color& color, const Color& secColor, int* key);
		virtual void draw() override;
		_NODISCARD bool isActive() const;
	private:
		std::string m_title;
		bool m_active;
		Color m_color;
		Color m_secColor;
		int* m_key;
	};
	// a very basic slider that represent some percentage or number to dynamically choice in range between
	class Slider : public Element
	{
	public:
		Slider(const std::string& title, const int x, const int y, const int width, const int height,
			const Color& color, const Color& secColor, std::pair<float, float> minMax, float* value);
		virtual void draw() override;
		_NODISCARD bool isActive() const;
	private:
		std::string m_title;
		bool m_active;
		Color m_color;
		Color m_secColor;
		std::pair<float, float> m_minMax;
		float* m_value;
	};

	// color picker with already made all colors by hsl looping. Alternatively you can draw it from file
	class ColorPicker : public Element
	{
	public:
		//ColorPicker(const std::string& title);
		virtual void draw() override;
		bool isActive() const;
	private:
		static std::unique_ptr<Color[]> m_gradient;
		static std::unique_ptr<Color[]> m_alphaBar;
		static std::unique_ptr<Color[]> m_hueBar;

		static int m_gradientID;
		static int m_textureALPHAID;
		static int m_textureHUEID;
		std::string m_title;
		bool m_active;
		Color getColorFromPos(const int x, const int y);
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
		inline bool grabbingWindow = false;
	}
	bool isKeyDown(const short key);
	bool isKeyPressed(const short key);
	void resetGlobals();

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
	inline Button button{ "Button", 20, 80, 50, 20, Color(50, 120, 220, 255), Color(50, 120, 180, 255),
		[]()
		{
			interfaces::console->consoleColorPrintf(Colors::Green, "Siema\n");
		}
	};
	inline int selected = -1;
	inline GroupBox group{ "This is a group", 20, 200, 150, 20, {"First", "Second", "Third", "Fourth", "Fith"}, Color(30, 30, 30, 255), Color(40, 40, 40, 255), &selected};
	inline std::vector<bool> options = { false, false, false, false, false, false, false, false};
	inline MultiBox multi{ "This is a multibox", 20, 300, 150, 20, {"First", "Second", "Third", "Fourth", "Fith", "Sixth", "Seventh", "Eith"},
		Color(30, 30, 30, 255), Color(40, 40, 40, 255), &options};
	inline std::string t = {};
	inline TextInput text{ "This is a textinput", 20, 120, 150, 20, Color(30, 30, 30, 255), Color(40, 40, 40, 255), &t };
	inline float slided = 0.0f;
	inline Slider slider{ "This is a slider", 20, 240, 150, 7, Colors::LightBlue, Color(30, 30, 30, 255), {0.0f, 100.0f}, &slided };
	inline int bind = 0;
	inline KeyHolder holder{ "This is a keyholder", 150, 50, 5, 8, Colors::White, Colors::Black, &bind };
	// not finished
	inline ColorPicker picker;

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