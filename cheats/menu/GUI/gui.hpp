#pragma once

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/278728-drawing-images-isurface-embedded-resource.html
// this helped with idea of library and using resource file

class TextureHolder
{
public:
	TextureHolder() = default;
	/// <summary>
	/// Constructor for images being drawn using surface
	/// </summary>
	/// <param name="resourceID">this will only work when given ID is somewhere in resource file</param>
	TextureHolder(const unsigned short resourceID);

	void draw(const int x, const int y);

protected:
	int m_textureID;
	size_t m_width, m_height;
};

enum Ranges
{
	GET_X = 1,
	GET_Y,
	GET_W,
	GET_H
};

namespace GUI
{
	// base class for menu gui
	class Element
	{
	public:
		static bool isMouseInRange(const int x, const int y, const int width, const int height);
		void setPos(const int x, const int y, const int width, const int height);
	protected:
		int m_X, m_Y;
		int m_width, m_height;
		// main drawing
		virtual void draw() = 0;
		// update the pos, when menu is moved
		virtual void update() = 0;
	};
	// class to draw basic window to represent gui. simple rect
	class Window : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
	// class to add tabs, it's like a button, but some things are different
	class Tab : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
	// button, it has the option to turn off/on the feature
	class Button : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
	// button but as a list with options
	class GroupBox : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
	// input that can go for numbers or letters
	class Input : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
	// a very basic slider that represent some percentage or number to dynamically choice in range between
	class Slider : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
	// the special case for this GUI, color picker will be based on image, since it will look nicer and easier to make
	class ColorPicker : public Element
	{
	public:
		virtual void draw() override;
		virtual void update() override;
	};
}

namespace test
{
	void run();
	void init();
	inline TextureHolder fatchad;
}