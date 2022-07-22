#pragma once
#pragma warning(disable: 26495)

#include "../../SDK/ISurface.hpp"
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/Vector2D.hpp"
#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <any>
#include <deque>

class Color;
class Resource;

struct Box
{
	float x, y, w, h;
};

struct Box3D
{
#ifdef SURFACE_DRAWING
	std::array<Vector2D, 8> points;

	Vector2D topleft;
	Vector2D topright;
	Vector2D bottomleft;
	Vector2D bottomright;
#else
	std::array<ImVec2, 8> points;

	ImVec2 topleft;
	ImVec2 topright;
	ImVec2 bottomleft;
	ImVec2 bottomright;
#endif
};

namespace fonts
{
	inline unsigned long tahoma;
	inline unsigned long franklinGothic;
	inline unsigned long verdana;
}

// rendering with game's engine
class SurfaceRender
{
public:
	void init();

	_NODISCARD unsigned long  __createFont(const char* fontName, const int size, const int weight, const unsigned long flags);

	void drawLine(const int x, const int y, const int x2, const int y2, const Color& color);
	void drawLine(const Vector2D& start, const Vector2D& end, const Color& color);
	void drawRect(const int x, const int y, const int w, const int h, const Color& color);
	void drawRect(const Vector2D& start, const Vector2D& end, const Color& color);
	void drawRectFilled(const int x, const int y, const int w, const int h, const Color& color);
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/181578-draw-rounded-box-static-dynamic.html
	void drawRoundedRect(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color);
	void drawRoundedRectFilled(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color);
	void drawCircle(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircle3D(const Vector& pos, const int radius, const int points, const Color& color);
	void drawFilledCircle3D(const Vector& pos, const int radius, const int points, const Color& color);
	void drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color);
	void drawTriangleFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color);
	void drawQuad(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawQuadFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color);
	void drawPolyLine(int* x, int* y, const int count, const Color& color);
	void drawPolyLine(const int count, Vertex_t* verts, const Color& color);
	void drawPolyGon(const int count, Vertex_t* verts, const Color& color, const bool clipped = true);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal, bool blend = true);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, const Color& third, bool horizontal, bool blend = true);
	void text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, const Color& color);
	void text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color);
	void textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...);
	[[deprecated("You should only read comments from there, better results are with ImGuiRender class")]]
	void drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled = false);
	// percent should be passed in 0.0-1.0 range, credits for helping Carlos1216
	void drawProgressRing(const int x, const int y, float radius, const int points, float percent, const float thickness, const Color& color);
	// width only
	_NODISCARD int getTextSize(const unsigned long font, const std::string& text);
	// width and height
	_NODISCARD Vector2D getTextSizeXY(const unsigned long font, const std::string& text);
	_NODISCARD bool worldToScreen(const Vector& in, Vector& out);
	_NODISCARD bool worldToScreen(const Vector& in, Vector2D& out);
	void initNewTexture(int& id, Color* RGBA, const int w, const int h);
	void initNewTexture(int& id, unsigned char* RGBA, const int w, const int h);
	void drawImage(const Resource& res, const int x, const int y, const int w, const int h, const Color& color = Colors::White);
};

inline SurfaceRender surfaceRender;

#include "../../dependencies/ImGui/imgui_impl_dx9.h"

namespace ImFonts
{
	inline ImFont* tahoma14;
	inline ImFont* tahoma20;
	inline ImFont* franklinGothic30;
	inline ImFont* franklinGothic12;
	inline ImFont* verdana12;
	inline ImFont* icon;
}

enum class DrawType : size_t
{
	NONE = 0,
	LINE,
	RECT,
	RECT_FILLED,
	RECT_GRADIENT,
	RECT_MULTICOLOR,
	CIRCLE,
	CIRCLE_FILLED,
	CIRCLE_3D,
	CIRCLE_3D_FILLED,
	TRIANGLE,
	TRIANGLE_FILLED,
	QUAD,
	QUAD_FILLED,
	QUAD_MULTICOLOR,
	POLYGON,
	POLYGON_FILLED,
	POLYGON_MULTICOLOR,
	TEXT,
	TEXT_SIZE,
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
	ImU32 m_color2; // right up
	ImU32 m_color3; // right bottom
	ImU32 m_color4; // left bottom
	ImDrawFlags m_flags;
	float m_thickness;
	float m_rounding = 0.0f;
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
	// circle 3D + outline for filled
	CircleObject_t(const Vector& pos, const std::vector<ImVec2>& pointsVec, float radius, int segments, ImU32 color, ImU32 outline, ImDrawFlags flags, float thickness)
		: m_pos{ pos }, m_pointsVec{ pointsVec }, m_radius{ radius }, m_segments{ segments }, m_color{ color }, m_outline{ outline }, m_flags{ flags }, m_thickness{ thickness }
	{}

	ImVec2 m_centre;
	Vector m_pos; // 3d
	std::vector<ImVec2> m_pointsVec; // 3d
	float m_radius;
	ImU32 m_color;
	ImU32 m_outline;
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
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_p4{ p4 }, m_color{ color }, m_thickness{ thickness }
	{}
	// filled quad
	QuadObject_t(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color)
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_p4{ p4 }, m_color{ color }
	{}
	// multicolors
	QuadObject_t(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4)
		: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_p4{ p4 }, m_color1{ color1 }, m_color2{ color2 }, m_color3{ color3 }, m_color4{ color4 }
	{}

	ImVec2 m_p1;
	ImVec2 m_p2;
	ImVec2 m_p3;
	ImVec2 m_p4;
	ImU32 m_color;
	ImU32 m_color1; // left up / normal color
	ImU32 m_color2; // right up
	ImU32 m_color3; // right bottom
	ImU32 m_color4; // left bottom
	ImDrawFlags m_flags;
	float m_thickness;
};

struct PolygonObject_t
{
	// normal polyline/polygon
	PolygonObject_t(const std::vector<ImVec2>& verts, ImU32 color, ImDrawFlags flags, float thickness)
		: m_count{ verts.size() }, m_verts{ verts }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }
	{}
	// filled polygon
	PolygonObject_t(const std::vector<ImVec2>& verts, ImU32 color)
		: m_count{ verts.size() }, m_verts{ verts }, m_color{ color }
	{}
	// filled polygon multicolor
	PolygonObject_t(const std::vector<ImVec2>& verts, const std::vector<ImU32>& colors)
		: m_count{ verts.size() }, m_verts{ verts }, m_colors{ colors }
	{}

	size_t m_count;
	std::vector<ImVec2> m_verts;
	std::vector<ImU32> m_colors;
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
	// text with size
	TextObject_t(float fontSize, ImFont* font, const ImVec2& pos, ImU32 color, const std::string& text, bool dropShadow, bool centered)
		: m_size{ fontSize }, m_font{ font }, m_pos{ pos }, m_color{ color }, m_text{ text }, m_dropShadow{ dropShadow }, m_centred{ centered }
	{}

	ImVec2 m_pos;
	ImFont* m_font;
	std::string m_text;
	ImU32 m_color;
	bool m_dropShadow;
	bool m_centred;
	float m_size = -1.0f;
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
	void drawLine(const ImVec2& start, const ImVec2& end, const Color& color, const float thickness = 1.0f);
	void drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags = 0);
	void drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags = 0);
	void drawRectFilledMultiColor(const float x, const float y, const float w, const float h,
		const Color& colUprLeft, const Color& colUprRight, const Color& colBotRight, const Color& colBotLeft);
	void drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness = 1.0f);
	void drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color);
	void drawCircle3D(const Vector& pos, const float radius, const int points, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& outline, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DFilled(const Vector& pos, const float radius, const int points, const Color& color, const Color& outline, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DFilledTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const Color& outline, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color, const float thickness = 1.0f);
	void drawTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color);
	void drawTrianglePoly(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color);
	void drawQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const Color& color, const float thickness = 1.0f);
	void drawQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const Color& color);
	void drawQuadFilledMultiColor(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4,
		const Color& colUprLeft, const Color& colUprRight, const Color& colBotRight, const Color& colBotLeft);
	void drawPolyLine(const std::vector<ImVec2>& verts, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawPolyGon(const std::vector<ImVec2>& verts, const Color& color);
	// for performace and reduce looping pass by using U32()
	void drawPolyGonMultiColor(const std::vector<ImVec2>& verts, const std::vector<ImU32>& colors);
	void drawGradient(const float x, const float y, const float w, const float h, const Color& first, const Color& second, bool horizontal);
	void text(const float x, const float y, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow = true);
	void text(const float x, const float y, const float fontSize, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow = true);
	void text(const float x, const float y, ImFont* font, const std::wstring& text, const bool centered, const Color& color, const bool dropShadow = true);
	void textf(const float x, const float y, ImFont* font, const bool centered, const Color& color, const bool dropShadow, const char* fmt, ...);
	// pass pos from world, you will often pass width.x == width.y
	// width.x -> pass starting width at front
	// width.y -> pass width of the box, between points at side
	void drawBox3D(const Vector& pos, const ImVec2& width, const float height, const Color& color, bool outlined = false, const float thickness = 2.0f);
	// pass pos from world, you will often pass width.x == width.y
	// width.x -> pass starting width at front
	// width.y -> pass width of the box, between points at side
	void drawBox3DFilled(const Vector& pos, const ImVec2& width, const float height, const Color& color, const Color& filling = Colors::Grey, bool outlined = false, const float thickness = 2.0f);
	void drawCone(const Vector& pos, const float radius, const int points, const float size, const Color& colCircle, const Color& colCone, const ImDrawFlags flags = 1, const float thickness = 1.0f);

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
	void drawSphere(const Vector& pos, float radius, float angleSphere, const Color& color);
	// no need for making it in mutex
	void drawImage(ImDrawList* draw, const Resource& res, const float x, const float y, const float w, const float h, const Color& color = Colors::White, float rounding = 0.0f, ImDrawFlags flags = ImDrawCornerFlags_All);
	_NODISCARD ImVec2 getTextSize(ImFont* font, const std::string& text);
	_NODISCARD bool worldToScreen(const Vector& in, Vector& out);
	_NODISCARD bool worldToScreen(const Vector& in, Vector2D& out);
	_NODISCARD bool worldToScreen(const Vector& in, ImVec2& out);

	// add to present
	void renderPresent(ImDrawList* draw);
	// add to surface
	void addToRender(const std::function<void()>& fun);
private:
	// add to surface
	void clearData();
	// add to surface
	void swapData();
	std::deque<DrawObject_t> m_drawData;
	std::deque<DrawObject_t> m_drawDataSafe;
	std::shared_mutex m_mutex;
};

inline ImGuiRender imRender;

// simple wrapper, use for anything that is not needed for w2s, and by itself draws inside destination window
class ImGuiRenderWindow
{
public:
	// also corrects the pos!
	void addList();
	// there is no need, very optional method
	void end();

	void drawLine(const float x, const float y, const float x2, const float y2, const Color& color, const float thickness = 1.0f);
	void drawLine(const ImVec2& start, const ImVec2& end, const Color& color, const float thickness = 1.0f);
	void drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags = 0);
	void drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags = 0, const float thickness = 1.0f);
	void drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags = 0);
	void drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness = 1.0f);
	void drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color);
	void drawPolyLine(const int count, ImVec2* verts, const Color& color, ImDrawFlags flags = 0, float thickness = 1.0f);
	void drawText(const float x, const float y, const float size, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow = true);
	void drawTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color);
	void drawProgressRing(const float x, const float y, const float radius, const int points, const float angleMin, float percent, const float thickness, const Color& color, const ImDrawFlags flags = 0);

	// remember it's a "cursor window pos"
	ImVec2 getPos() const { return m_pos; }
	ImVec2 getRect() const { return m_rect; }
	float getWidth() const { return m_rect.x; }
	float getHeight() const { return m_rect.y; }
	ImDrawList* getDrawList() const { return m_drawing; }
private:
	ImVec2 m_pos; // additional pos, it corrects the drawing pos, so we can pass literal destination to window, see comment below
	ImVec2 m_rect; // width and height
	ImDrawList* m_drawing;
};

inline ImGuiRenderWindow imRenderWindow;

/*
* This is out destination window
*	-------------------------	lets say it's 30px x 4
*	|						|
*	|						|
*	|						|
*	|						|
*	-------------------------
*	Now, using the for eg: drawRect(x1, y1, x2, y2) -> the position to draw will be added for us automatically
*	So you pass position that is inside the window, imagine this window as your new screen and you are based on passing coords to it.
*/