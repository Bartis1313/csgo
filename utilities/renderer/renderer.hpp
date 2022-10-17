#pragma once

#include "BBox.hpp"

#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <any>
#include <deque>
#include <array>

#include <SDK/math/Vector.hpp>
#include <dependencies/ImGui/imgui.h>

class Color;
class Resource;
class Entity_t;
struct Vertex_t;

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

	[[nodiscard]] unsigned long  __createFont(const char* fontName, const int size, const int weight, const unsigned long flags);

	void drawLine(const int x, const int y, const int x2, const int y2, const Color& color);
	void drawLine(const Vec2& start, const Vec2& end, const Color& color);
	void drawRect(const int x, const int y, const int w, const int h, const Color& color);
	void drawRect(const Vec2& start, const Vec2& end, const Color& color);
	void drawRectFilled(const int x, const int y, const int w, const int h, const Color& color);
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/181578-draw-rounded-box-static-dynamic.html
	void drawRoundedRect(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color);
	void drawRoundedRectFilled(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color);
	void drawCircle(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color);
	void drawCircle3D(const Vec3& pos, const int radius, const int points, const Color& color);
	void drawFilledCircle3D(const Vec3& pos, const int radius, const int points, const Color& color);
	void drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color);
	void drawTriangleFilled(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color);
	void drawQuad(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& p4, const Color& color);
	void drawQuadFilled(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& p4, const Color& color);
	void drawPolyLine(int* x, int* y, const int count, const Color& color);
	void drawPolyLine(const int count, Vertex_t* verts, const Color& color);
	void drawPolyGon(const int count, Vertex_t* verts, const Color& color, const bool clipped = true);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal, bool blend = true);
	void drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, const Color& third, bool horizontal, bool blend = true);
	void text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, const Color& color);
	void text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color);
	void textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...);
	[[deprecated("You should only read comments from there, better results are with ImGuiRender class")]]
	void drawBox3D(const std::array<Vec3, 8>& box, const Color& color, bool filled = false);
	// percent should be passed in 0.0-1.0 range, credits for helping Carlos1216
	void drawProgressRing(const int x, const int y, float radius, const int points, float percent, const float thickness, const Color& color);
	// width only
	[[nodiscard]] int getTextSize(const unsigned long font, const std::string& text);
	// width and height
	[[nodiscard]] Vec2 getTextSizeXY(const unsigned long font, const std::string& text);
	[[nodiscard]] bool worldToScreen(const Vec3& in, Vec2& out);
	void initNewTexture(int& id, Color* RGBA, const int w, const int h);
	void initNewTexture(int& id, unsigned char* RGBA, const int w, const int h);
	void drawImage(const Resource& res, const int x, const int y, const int w, const int h, const Color& color);
};

inline SurfaceRender surfaceRender;

struct ImGuiIO;
struct ImFont;

namespace ImFonts
{
	inline ImFont* tahoma14;
	inline ImFont* tahoma20;
	inline ImFont* franklinGothic30;
	inline ImFont* franklinGothic12;
	inline ImFont* verdana12;
	inline ImFont* icon;
}

#include <memory>
#include "structures.hpp"

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
	void drawCircle3D(const Vec3& pos, const float radius, const int points, const Color& color, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DTraced(const Vec3& pos, const float radius, const int points, void* skip, const Color& outline, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DFilled(const Vec3& pos, const float radius, const int points, const Color& color, const Color& outline, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawCircle3DFilledTraced(const Vec3& pos, const float radius, const int points, void* skip, const Color& color, const Color& outline, const ImDrawFlags flags = 1, const float thickness = 1.0f);
	void drawTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color, const float thickness = 1.0f);
	void drawTriangle(const float x, const float y, const float w, const float h, const Color& color, const float angle, const float thickness = 1.0f);
	void drawTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color);
	void drawTriangleFilled(const float x, const float y, const float w, const float h, const float angle, const Color& color);
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
	void drawBox3D(const Vec3& pos, const float width, const float height, const Color& color, bool outlined = false, const float thickness = 2.0f);
	void drawBox3DFilled(const Vec3& pos, const float width, const float height, const Color& color, const Color& filling, bool outlined = false, const float thickness = 2.0f);
	void drawCone(const Vec3& pos, const float radius, const int points, const float size, const Color& colCircle, const Color& colCone, const ImDrawFlags flags = 1, const float thickness = 1.0f);

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
	void drawSphere(const Vec3& pos, float radius, float angleSphere, const Color& color);
	[[nodiscard]] ImVec2 getTextSize(ImFont* font, const std::string& text);
	[[nodiscard]] bool worldToScreen(const Vec3& in, ImVec2& out);

	// add to present
	void renderPresent(ImDrawList* draw);
	// add to surface
	void addToRender(const std::function<void()>& fun);
private:
	// add to surface
	void clearData();
	// add to surface
	void swapData();
	std::deque<std::shared_ptr<drawing::Draw>> m_drawData;
	std::deque<std::shared_ptr<drawing::Draw>> m_drawDataSafe;
	std::shared_mutex m_mutex;
};

inline ImGuiRender imRender;

// todo: deprecate this
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
	void drawTriangleFilled(const float x, const float y, const float w, const float h, const float angle, const Color& color);
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