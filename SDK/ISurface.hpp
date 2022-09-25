#pragma once

#include "math/Vector2D.hpp"
#include "Color.hpp"

#include <utilities/vfunc.hpp>

struct Vertex_t
{
	Vertex_t() = default;
	constexpr Vertex_t(const Vector2D& pos, const Vector2D& coord = Vector2D{})
		: m_Position{ pos }, m_TexCoord{ coord }
	{}

	Vector2D m_Position;
	Vector2D m_TexCoord;
};

class ISurface
{
public:
	VFUNC(void, drawSetColor, 14, (SDKColor color), (this, color));
	VFUNC(void, setTextColor, 24, (SDKColor color), (this, color));
	VFUNC(void, drawTexturedPolygon, 106, (int n, Vertex_t* vertice, bool clip = true), (this, n, vertice, clip));
	VFUNC(void, drawFilledRectangle, 16, (int x, int y, int w, int h), (this, x, y, x + w, y + h));
	VFUNC(void, drawSetTexture, 38, (int id), (this, id));
	VFUNC(void, setTextureRGBA, 37, (int id, const unsigned char* rgba, int wide, int tall), (this, id, rgba, wide, tall));
	VFUNC(void, setTextureRGBA, 37, (int id, Color* rgba, int wide, int tall), (this, id, rgba, wide, tall));
	VFUNC(int, createNewTextureID, 43, (bool procedural = false), (this, procedural));
	VFUNC(bool, deleteTextureID, 39, (int id), (this, id));
	VFUNC(void, drawOutlinedRect, 18, (int x, int y, int w, int h), (this, x, y, x + w, y + h));
	VFUNC(void, drawLine, 19, (int x1, int y1, int x2, int y2), (this, x1, y1, x2, y2));
	VFUNC(void, drawPolyLine, 20, (int* px, int* py, int numPoints), (this, px, py, numPoints));
	VFUNC(void, drawTextFont, 23, (unsigned long font), (this, font));
	VFUNC(void, drawTextPos, 26, (int x, int y), (this, x, y));
	VFUNC(void, drawRenderText, 28, (const wchar_t* text, int textLen), (this, text, textLen, 0));
	VFUNC(unsigned long, fontCreate, 71, (), (this));
	VFUNC(void, setFontGlyph, 72, (unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags),
		(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0));
	VFUNC(void, getTextSize, 79, (unsigned long font, const wchar_t* text, int& wide, int& tall),
		(this, font, text, std::ref(wide), std::ref(tall)));
	VFUNC(void, unlockCursor, 66, (), (this));
	VFUNC(void, setClipRect, 147, (int x, int y, int w, int h), (this, x, y, w, h));
	VFUNC(void, drawFilledFadeRect, 123, (int x, int y, int w, int h, unsigned int alpha1, unsigned int alpha2, bool horizontal),
		(this, x, y, x + w, y + h, alpha1, alpha2, horizontal));
	VFUNC(void, drawColorText, 163, (unsigned long font, int x, int y, Color col, int alpha, const char* text),
		(this, font, x, y, col, alpha, text));
	VFUNC(void, getScreenSize, 44, (int& width, int& height),
		(this, std::ref(width), std::ref(height)));
	VFUNC(void, drawOutlinedCircle, 103, (int x, int y, int radius, int points), (this, x, y, radius, points));
	VFUNC(void, drawTextureFromName, 36, (int id, const char* name, int filter, bool reload = 0), (this, id, name, filter, reload));
	VFUNC(void, getCursor, 100, (int& x, int& y), (this, std::ref(x), std::ref(y)));
	VFUNC(void, drawTexturedRect, 41, (int x, int y, int width, int height), (this, x, y, width, height));
	VFUNC(void, playSound, 82, (const char* name), (this, name));
	VFUNC(bool, isTextureValid, 42, (int id), (this, id));
};