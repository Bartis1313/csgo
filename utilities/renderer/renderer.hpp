#pragma once
#include "../../SDK/ISurface.hpp"
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/Vector2D.hpp"

struct Box
{
	int x, y, w, h;
};

namespace fonts
{
	inline unsigned long tahoma;
	inline unsigned long smalle;
	inline unsigned long espBar;
}

namespace render
{
	void init();

	unsigned long  __createFont(const char* fontName, const int size, const int weight, const unsigned long flags);

	void drawLine(const int x, const int y, const int x2, const int y2, const Color& color);
	void drawLine(const Vector2D& start, const Vector2D& end, const Color& color);
	void drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color);
	void drawOutlineRect(const Vector2D& start, const Vector2D& end, const Color& color);
	void drawFilledRect(const int x, const int y, const int w, const int h, const Color& color);
	void drawCircle(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircle3D(const Vector& pos, const int radius, const int points, const Color& color);
	void drawFilledCircle3D(const Vector& pos, const int radius, const int points, const Color& color);
	void drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color);
	void drawTrapezFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawTrapezOutline(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawPolyLine(int* x, int* y, const int count, const Color& color);
	void drawPolyLine(const int count, Vertex_t* verts, const Color& color);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal);
	void text(const int x, const int y, const unsigned long font, const wchar_t *text, const bool centered, const Color& color);
	void text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color);
	void textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...);
	void drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled = false);
	int getTextSize(const unsigned long font, const std::string& text);
	bool worldToScreen(const Vector& in, Vector& out);
}
