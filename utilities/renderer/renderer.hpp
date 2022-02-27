#pragma once
#include "../../SDK/ISurface.hpp"
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/Vector2D.hpp"
#include "../../dependencies/ImGui/imgui.h"
#include <string>

struct Box
{
	int x, y, w, h;
};

enum FontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

// make it as a struct to make calculations for Y padding offset for gui, etc
namespace fonts
{
	inline unsigned long tahoma;
	inline unsigned long smalle;
	inline unsigned long espBar;
	inline unsigned long menuFont;
}

namespace ImFonts
{
	inline ImFont* tahoma;
	inline ImFont* espBar;
	inline ImFont* menuFont;
}

class Render
{
public:
	void init();

	_NODISCARD unsigned long  __createFont(const char* fontName, const int size, const int weight, const unsigned long flags);

	void drawLine(const int x, const int y, const int x2, const int y2, const Color& color);
	void drawLine(const Vector2D& start, const Vector2D& end, const Color& color);
	void drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color);
	void drawOutlineRect(const Vector2D& start, const Vector2D& end, const Color& color);
	void drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color, const Color& colorOutline);
	void drawFilledRect(const int x, const int y, const int w, const int h, const Color& color);
	void drawFilledRect(const int x, const int y, const int w, const int h, const Color& color, const Color& colorOutline);
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/181578-draw-rounded-box-static-dynamic.html
	void drawRoundedRect(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color);
	void drawCircle(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircle3D(const Vector& pos, const int radius, const int points, const Color& color);
	void drawFilledCircle3D(const Vector& pos, const int radius, const int points, const Color& color);
	void drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color);
	void drawTrapezFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawTrapezOutline(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawPolyLine(int* x, int* y, const int count, const Color& color);
	void drawPolyLine(const int count, Vertex_t* verts, const Color& color);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal, bool blend = true);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, const Color& third, bool horizontal, bool blend = true);
	void text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, const Color& color);
	void text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color);
	void textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...);
	void drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled = false);
	// percent should be passed in 0-100 range, credits for helping Carlos1216
	void drawProgressRing(const int x, const int y, float radius, const int points, float percent, const float thickness, const Color& color);
	// width only
	_NODISCARD int getTextSize(const unsigned long font, const std::string& text);
	// width and height
	_NODISCARD Vector2D getTextSizeXY(const unsigned long font, const std::string& text);
	// use this function for any rendering struct
	_NODISCARD bool worldToScreen(const Vector& in, Vector& out);
	void initNewTexture(int& id, Color* RGBA, const int w, const int h);
	void initNewTexture(int& id, unsigned char* RGBA, const int w, const int h);
	// color argument is very sometimes needed, because texture is mostly supposed to be all filled
	void drawFromTexture(const int id, const int x, const int y, const int w, const int h, const Color& color);
} inline render;

// rendering supported by dear ImGui, few changes and new functions comparing to surface draw
class ImGuiRender
{
public:

	void init(ImGuiIO& io);

	void drawLine(const float x, const float y, const float x2, const float y2, const Color& color, const float thickness = 1.0f);
	void drawLine(const Vector2D& start, const Vector2D& end, const Color& color, const float thickness = 1.0f);
	void drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags = 0);
	void drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags = 0);
	void drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness = 1.0f);
	void drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color);
	void drawCircle3D(const Vector& pos, const float radius, const int points, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DFilled(const Vector& pos, const float radius, const int points, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DFilledTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color, const float thickness = 1.0f);
	void drawTriangleFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color);
	void drawTrapezian(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color, const float thickness = 1.0f);
	void drawTrapezianFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawPolyLine(const int count, ImVec2* verts, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawGradient(const float x, const float y, const float w, const float h, const Color& first, const Color& second, bool horizontal);
	void text(const float x, const float y, ImFont* font, const std::string& text, const bool centered, const Color& color, FontFlags flags = FONTFLAG_OUTLINE);
	void text(const float x, const float y, ImFont* font, const std::wstring& text, const bool centered, const Color& color, FontFlags flags = FONTFLAG_OUTLINE);
	void textf(const float x, const float y, ImFont* font, const bool centered, const Color& color, FontFlags flags, const char* fmt, ...);
	void drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled = false, const float thickness = 3.0f);
	void drawArc(const float x, const float y, float radius, const int points, float angleMin, float angleMax, const float thickness, const Color& color, const ImDrawFlags flags = 0);
	void drawProgressRing(const float x, const float y, const float radius, const int points, const float angleMin, float percent, const float thickness, const Color& color, const ImDrawFlags flags = 0);
	_NODISCARD ImVec2 getTextSize(ImFont* font, const std::string& text);
	_NODISCARD bool worldToScreen(const Vector& in, Vector& out);
public:
	ImDrawList* m_drawing;
} inline imRender;
