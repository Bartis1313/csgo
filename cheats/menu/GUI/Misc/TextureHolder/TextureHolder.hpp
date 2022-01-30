#pragma once
#include <vector>
#include "../../../../../resource.h" // here because when using this file, we should get some ready file for resource id
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/278728-drawing-images-isurface-embedded-resource.html
// this helped with idea of library and using resource file

class TextureHolder final
{
public:
	TextureHolder() = default;
	/// <summary>
	/// Constructor for images being drawn using surface,
	/// remember: width and height are same as the original image sizes
	/// </summary>
	/// <param name="resourceID">This will only work when given ID is somewhere in resource file</param>
	/// <param name="x">Where to draw X</param>
	/// <param name="y">Where to draw Y</param>
	TextureHolder(const unsigned short resourceID);

	void draw(const int x, const int y);

	_NODISCARD size_t getWidth() const { return m_width; }
	_NODISCARD size_t getHeight() const { return m_height; }
	_NODISCARD int getTextureID() const { return m_textureID; }
private:
	int m_textureID;
	size_t m_width, m_height;
};