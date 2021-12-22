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
	struct Area
	{
		int x, y, w, h;
	};

	class GUI
	{
	public:
		Area getRanges();
		void initAreaNormal(int x, int y, int w, int h)
		{
			m_area = Area{ x, y, w, h };
		}
		bool isMouseInRange();
	protected:
		Area m_area;
		// main drawing
		virtual void draw() = 0;
		// update the pos, when menu is moved
		virtual void update() = 0;
	};
}

namespace test
{
	void run();
	void init();
	inline TextureHolder fatchad;
}