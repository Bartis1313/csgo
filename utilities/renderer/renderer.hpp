#pragma once
#pragma warning(disable: 26495)

#include "../../SDK/ISurface.hpp"
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/Vector2D.hpp"
#include "../../dependencies/ImGui/imgui.h"
#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <any>
#include <deque>

class Color;

struct Box
{
	float x, y, w, h;
};

namespace fonts
{
	inline unsigned long tahoma;
	inline unsigned long smalle;
	inline unsigned long espBar;
	inline unsigned long menuFont;
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
} inline [[deprecated("This is not updated anymore! To turn off this warning, remove attribute and edit surface drawing yourself, and replace Color to SDKColor!")]] render;

namespace ImFonts
{
	inline ImFont* tahoma;
	inline ImFont* espBar;
	inline ImFont* menuFont;
}

enum class DrawType : int
{
	NONE = 0,
	LINE,
	RECT,
	RECT_FILLED,
	RECT_GRADIENT,
	CIRCLE,
	CIRCLE_FILLED,
	CIRCLE_3D,
	CIRCLE_3D_FILLED,
	TRIANGLE,
	TRIANGLE_FILLED,
	QUAD,
	QUAD_FILLED,
	POLYGON,
	POLYGON_FILLED,
	TEXT,
	ARC,
};

struct DrawObject_t
{
	DrawObject_t(const DrawType type, std::any&& obj)
		: m_type{ type }, m_drawingObj{ obj }
	{}

	DrawType m_type = DrawType::NONE;
	std::any m_drawingObj = {};
};

struct LineObject_t
{
	ImVec2 m_start;
	ImVec2 m_end;
	ImU32 m_color;
	float m_thickness;
};

struct RectObject_t
{
	// normal rect
	RectObject_t(const ImVec2& min, const ImVec2& max, ImU32 color, float rounding, ImDrawFlags flags, float thickness)
		: m_min{ min }, m_max{ max }, m_color1{ color }, m_rounding{ rounding }, m_flags{ flags }, m_thickness{ thickness }
	{}
	// filled rect
	RectObject_t(const ImVec2& min, const ImVec2& max, ImU32 color, float rounding, ImDrawFlags flags)
		: m_min{ min }, m_max{ max }, m_color1{ color }, m_rounding{ rounding }, m_flags{ flags }
	{}
	// gradient - 2 colors
	RectObject_t(const ImVec2& min, const ImVec2& max, ImU32 color1, ImU32 color2, bool horizontal)
		: m_min{ min }, m_max{ max }, m_color1{ color1 }, m_color2{ color2 }, m_horizontal{ horizontal }
	{}
	// gradient - multicolor 4 colors
	RectObject_t(const ImVec2& min, const ImVec2& max, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4)
		: m_min{ min }, m_max{ max }, m_color1{ color1 }, m_color2{ color2 }, m_color3{ color3 }, m_color4{ color4 }
	{}

	ImVec2 m_min;
	ImVec2 m_max;
	ImU32 m_color1; // left up / normal color
	ImU32 m_color2; // right ip
	ImU32 m_color3; // left bottom
	ImU32 m_color4; // right bottom
	ImDrawFlags m_flags;
	float m_thickness;
	float m_rounding;
	bool m_horizontal;
};

struct CircleObject_t
{
	// normal circle 2D
	CircleObject_t(const ImVec2& centre, float radius, int segments, ImU32 color, float thickness)
		: m_centre{ centre }, m_radius{ radius }, m_segments{ segments }, m_color{ color }, m_thickness{ thickness }
	{}
	// filled circle 2D
	CircleObject_t(const ImVec2& centre, float radius, int segments, ImU32 color)
		: m_centre{ centre }, m_radius{ radius }, m_segments{ segments }, m_color{ color }
	{}
	// circle 3D
	CircleObject_t(const Vector& pos, const std::vector<ImVec2>& pointsVec, float radius, int segments, ImU32 color, ImDrawFlags flags, float thickness)
		: m_pos{ pos }, m_pointsVec{ pointsVec }, m_radius{ radius }, m_segments{ segments }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }
	{}

	ImVec2 m_centre;
	Vector m_pos; // 3d
	std::vector<ImVec2> m_pointsVec; // 3d
	float m_radius;
	ImU32 m_color;
	int m_segments;
	ImDrawFlags m_flags;
	float m_thickness;
};

struct TriangleObject_t
{
	// normal triangle
	TriangleObject_t(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 color, float thickness)
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_color{ color }, m_thickness{ thickness }
	{}
	// filled triangle
	TriangleObject_t(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 color)
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_color{ color }
	{}

	ImVec2 m_p1;
	ImVec2 m_p2;
	ImVec2 m_p3;
	ImU32 m_color;
	float m_thickness;
};

struct QuadObject_t
{
	// normal quad
	QuadObject_t(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color, float thickness)
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_color{ color }, m_thickness{ thickness }
	{}
	// filled quad
	QuadObject_t(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color)
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_color{ color }
	{}

	ImVec2 m_p1;
	ImVec2 m_p2;
	ImVec2 m_p3;
	ImVec2 m_p4;
	ImU32 m_color;
	ImDrawFlags m_flags;
	float m_thickness;
};

struct PolygonObject_t
{
	// normal polyline/polygon
	PolygonObject_t(int count, ImVec2* verts, ImU32 color, ImDrawFlags flags, float thickness)
		: m_count{ count }, m_verts{ verts }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }
	{}
	// filled polyline/polygon
	PolygonObject_t(int count, ImVec2* verts, ImU32 color)
		: m_count{ count }, m_verts{ verts }, m_color{ color }
	{}

	int m_count;
	ImVec2* m_verts;
	ImU32 m_color;
	ImDrawFlags m_flags;
	float m_thickness;
};

struct TextObject_t
{
	// normal text
	TextObject_t(ImFont* font, const ImVec2& pos, ImU32 color, const std::string& text, bool dropShadow, bool centered)
		: m_font{ font }, m_pos{ pos }, m_color{ color }, m_text{ text }, m_dropShadow{ dropShadow }, m_centred{ centered }
	{}

	ImVec2 m_pos;
	ImFont* m_font;
	std::string m_text;
	ImU32 m_color;
	bool m_dropShadow;
	bool m_centred;
};

struct ArcObject_t
{
	// classic arc, give 2 angles
	ArcObject_t(const ImVec2& centre, float radius, float aMin, float aMax, int segments, ImU32 color, ImDrawFlags flags, float thickness)
		: m_centre{ centre }, m_radius{ radius }, m_aMin{ aMin }, m_aMax{ aMax }, m_segments{ segments }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }
	{}
	// special arc, draws stops path by percentage amount
	ArcObject_t(const ImVec2& centre, float radius, float aMin, float aMax, int segments, ImU32 color, ImDrawFlags flags, float thickness, float percent)
		: m_centre{ centre }, m_radius{ radius }, m_aMin{ aMin }, m_aMax{ aMax }, m_segments{ segments }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }, m_percent{ percent }
	{}

	ImVec2 m_centre;
	float m_radius;
	float m_aMin;
	float m_aMax;
	int m_segments;
	ImU32 m_color;
	ImDrawFlags m_flags;
	float m_thickness;
	float m_percent;
};


// rendering supported by dear ImGui, few changes and new functions comparing to surface draw
// thread safe idea - full credits to qo0' as I couldn't really rebuild manually w2s with any success removing this weird stutter
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
	void drawPolyGon(const int count, ImVec2* verts, const Color& color);
	void drawGradient(const float x, const float y, const float w, const float h, const Color& first, const Color& second, bool horizontal);
	void text(const float x, const float y, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow = true);
	void text(const float x, const float y, ImFont* font, const std::wstring& text, const bool centered, const Color& color, const bool dropShadow = true);
	void textf(const float x, const float y, ImFont* font, const bool centered, const Color& color, const bool dropShadow, const char* fmt, ...);
	void drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled = false, const float thickness = 3.0f);

	/*
	* arcs - there are few problems with them. Especially you can see it when trying to do arc that is a full circle.
	* You will see that they infact are maybe filled, but few pixels are missing.
	* 1st fix - ghetto, but will maybe behave better for performance, when delta is 360 deg or for progress - percent is 100
	* then we make delta - 0.1f, same thing for percent. And then call ImDrawFlags_Closed.
	* No ghetto fix, is just to call it with many segments.
	* Correct me if you found any better solution, or explain it why.
	*/

	void drawArc(const float x, const float y, float radius, const int points, float angleMin, float angleMax, const float thickness, const Color& color, const ImDrawFlags flags = 0);
	void drawProgressRing(const float x, const float y, const float radius, const int points, const float angleMin, float percent, const float thickness, const Color& color, const ImDrawFlags flags = 0);
	_NODISCARD ImVec2 getTextSize(ImFont* font, const std::string& text);
	_NODISCARD bool worldToScreen(const Vector& in, Vector& out);
	_NODISCARD bool worldToScreen(const Vector& in, Vector2D& out);

	// add to present
	void renderPresent(ImDrawList* draw);
	// add to surface
	void clearData();
	// add to surface
	void swapData();
	// add to surface
	__declspec(noinline) void addToRender(const std::function<void()>& fun) { fun(); }

	std::deque<DrawObject_t> m_drawData;
	std::deque<DrawObject_t> m_drawDataSafe;
	std::shared_mutex m_mutex;
} inline imRender;
