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