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
	extern unsigned long tahoma;
	extern unsigned long smalle;
}

namespace render
{
	void init();

	//vgui::HFont __createFont(const char* fontName, int size, int weight, unsigned long flags);

	void drawLine(int x, int y, int x2, int y2, Color color);
	void drawLine(Vector2D start, Vector2D end, Color color);
	void drawOutlineRect(int x, int y, int w, int h, Color color);
	void drawOutlineRect(Vector2D start, Vector2D end, Color color);
	void drawFilledRect(int x, int y, int w, int h, Color color);
	void drawCircle(int x, int y, int radius, int points, Color color);
	void drawCircleFilled(int x, int y, int radius, int points, Color color);
	void drawTriangle(Vector2D p1, Vector2D p2, Vector2D p3, Color color);
	void drawTrapezFilled(Vector2D p1, Vector2D p2, Vector2D p3, Vector2D p4, Color color);
	void drawTrapezOutline(Vector2D p1, Vector2D p2, Vector2D p3, Vector2D p4, Color color);
	void drawPolyLine(int* x, int* y, const int count, Color color);
	void drawPolyLine(const int count, Vertex_t* verts, Color color);
	void drawPolygonOutline(const int count, Vertex_t* verts, Color color);
	void text(const int x, const int y, const unsigned long font, const wchar_t *text, const bool centered, Color color);
	void text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, Color color);
	[[deprecated("use render::text with std::format instead")]]
	void textf(const int x, const int y, const unsigned long font, const bool centered, Color color, const char* fmt, ...);

	void drawBox3D(Vector* box, const Color& color, bool filled = false);

	// get text size, it responses for esp distance format and more options
	int getTextSize(unsigned long font, const std::string& text);

	// for hack
	bool WorldToScreen(const Vector& in, Vector& out);
}
