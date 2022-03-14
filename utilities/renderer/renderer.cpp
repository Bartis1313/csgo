#include "renderer.hpp"
#include "../../SDK/interfaces/interfaces.hpp"
#include "../../dependencies/ImGui/imgui_freetype.h"
#include "../../dependencies/ImGui/imgui_internal.h"
#include "../math/math.hpp"
#include <filesystem>

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


unsigned long Render::__createFont(const char* fontName, const int size, const int weight, const unsigned long flags)
{
	auto result = interfaces::surface->fontCreate();
	interfaces::surface->setFontGlyph(result, fontName, size, weight, 0, 0, flags);
	return result;
}

void Render::init()
{
	fonts::tahoma = __createFont(XOR("Tahoma"), 14, 800, FONTFLAG_OUTLINE);
	fonts::smalle = __createFont(XOR("Tahoma"), 9, 800, FONTFLAG_ANTIALIAS);
	fonts::espBar = __createFont(XOR("Franklin Gothic"), 10, 300, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts::menuFont = __createFont(XOR("Verdana"), 12, 350, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	LOG(LOG_INFO, XOR("render init success"));
}

void Render::drawLine(const int x, const int y, const int x2, const int y2, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawLine(x, y, x2, y2);
}

void Render::drawLine(const Vector2D& start, const Vector2D& end, const Color& color)
{
	drawLine(start.x, start.y, end.x, end.y, color);
}

void Render::drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawOutlinedRect(x, y, w, h);
}

void Render::drawOutlineRect(const Vector2D& start, const Vector2D& end, const Color& color)
{
	drawOutlineRect(start.x, start.y, end.x, end.y, color);
}

void Render::drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color, const Color& colorOutline)
{
	drawOutlineRect(x, y, w, h, color);
	drawOutlineRect(x - 1, y - 1, w + 2, h + 2, colorOutline);
	drawOutlineRect(x + 1, y + 1, w - 2, h - 2, colorOutline);
}

void Render::drawFilledRect(const int x, const int y, const int w, const int h, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawFilledRectangle(x, y, w, h);
}

void Render::drawFilledRect(const int x, const int y, const int w, const int h, const Color& color, const Color& colorOutline)
{
	drawFilledRect(x, y, w, h, color);
	drawOutlineRect(x - 1, y - 1, w + 2, h + 2, colorOutline);
	drawOutlineRect(x + 1, y + 1, w - 2, h - 2, colorOutline);
}

void Render::drawRoundedRect(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color)
{
	if (numberOfVertices < 2)
		return;

	auto roundV = std::make_unique<Vertex_t[]>(4 * numberOfVertices);

	for (int i = 0; i < 4; i++)
	{
		int _x = x + ((i < 2) ? (w - radius) : radius);
		int _y = y + ((i % 3) ? (h - radius) : radius);

		for (int j = 0; j < numberOfVertices; j++)
		{
			float rad = DEG2RAD((90.f * i) + (j / static_cast<float>(numberOfVertices - 1)) * 90.f);

			roundV[(i * numberOfVertices) + j] = Vertex_t{ Vector2D{_x + radius * std::sin(rad), _y - radius * std::cos(rad)} };
		}
	}

	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawTexturedPolygon(4 * numberOfVertices, roundV.get());
}

void Render::drawCircle(const int x, const int y, const int radius, const int points, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawOutlinedCircle(x, y, radius, points);
}

void Render::drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color)
{
	std::vector<Vertex_t> verts = {};

	float step = std::numbers::pi_v<float> *2.0f / points;
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		verts.emplace_back(std::move(Vector2D(x + (radius * std::cos(angle)), y + (radius * std::sin(angle)))));
	}
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawTexturedPolygon(verts.size(), verts.data());
}

void Render::drawCircle3D(const Vector& pos, const int radius, const int points, const Color& color)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector start(radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z);
		Vector end(radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z);

		if (Vector screenStart, screenEnd; worldToScreen(start, screenStart) && worldToScreen(end, screenEnd))
		{
			drawLine(screenStart.x, screenStart.y, screenEnd.x, screenEnd.y, color);
		}
	}
}

void Render::drawFilledCircle3D(const Vector& pos, const int radius, const int points, const Color& color)
{
	Vector orignalW2S = {};
	if (!worldToScreen(pos, orignalW2S))
		return;

	Vector screenStart = {};
	float step = std::numbers::pi_v<float> *2.0f / points;
	Vector before = {};

	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector start(radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z);

		if (worldToScreen(start, screenStart) && !before.isZero())
		{
			drawLine(screenStart.x, screenStart.y, before.x, before.y, color);
			drawTriangle(Vector2D(orignalW2S.x, orignalW2S.y), Vector2D(screenStart.x, screenStart.y), Vector2D(before.x, before.y),
				Color(color.r(), color.g(), color.b(), color.a() / 4.0f));
		}
		before = screenStart;
	}
}

void Render::drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color)
{
	Vertex_t verts[] =
	{
		Vertex_t(p1),
		Vertex_t(p2),
		Vertex_t(p3)
	};

	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawTexturedPolygon(3, verts);
}

void Render::drawTrapezFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
{
	Vertex_t verts[] =
	{
		Vertex_t(p1),
		Vertex_t(p2),
		Vertex_t(p3),
		Vertex_t(p4)
	};

	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawTexturedPolygon(4, verts);
}

void Render::drawTrapezOutline(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
{
	Vertex_t verts[] =
	{
		Vertex_t(p1),
		Vertex_t(p2),
		Vertex_t(p3),
		Vertex_t(p4)
	};

	drawPolyLine(4, verts, color);
}

void Render::drawPolyLine(int* x, int* y, const int count, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawPolyLine(x, y, count);
}

void Render::drawPolyLine(const int count, Vertex_t* verts, const Color& color)
{
	static int x[300];
	static int y[300];

	for (int i = 0; i < count; i++)
	{
		x[i] = verts[i].m_Position.x;
		y[i] = verts[i].m_Position.y;
	}

	drawPolyLine(x, y, count, color);
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/189418-medical-attention-gradients-surface.html got fixed gradient blend from there
void Render::drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal, bool blend)
{
	auto gradient = [=](const Color& clr, bool reversed)
	{
		interfaces::surface->drawSetColor(clr);
		interfaces::surface->drawFilledFadeRect(
			x, y, w, h,
			reversed ? clr.a() : 0,
			reversed ? 0 : clr.a(),
			horizontal ? true : false);
	};

	auto blendColor = [](const Color& first, const Color& second, float t)
	{
		return Color(
			first.r() + t * (second.r() - first.r()),
			first.g() + t * (second.g() - first.g()),
			first.b() + t * (second.b() - first.b()),
			first.a() + t * (second.a() - first.a()));
	};

	if (blend)
		drawFilledRect(x, y, w, h, blendColor(first, second, 0.5f));
	gradient(first, true);
	gradient(second, false);
}

void Render::drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, const Color& third, bool horizontal, bool blend)
{
	auto gradient = [=](const Color& clr, bool reversed)
	{
		interfaces::surface->drawSetColor(clr);
		interfaces::surface->drawFilledFadeRect(
			x, y, w, h,
			reversed ? clr.a() : 0,
			reversed ? 0 : clr.a(),
			horizontal ? true : false);
	};

	auto blendColor = [](const Color& first, const Color& second, const Color& third, float t)
	{
		return Color(
			first.r() + t * (second.r() - third.r()),
			first.g() + t * (second.g() - third.g()),
			first.b() + t * (second.b() - third.b()),
			first.a() + t * (second.a() - third.a()));
	};

	if (blend)
		drawFilledRect(x, y, w, h, blendColor(first, second, third, 0.5f));
	gradient(first, true);
	gradient(second, false);
	gradient(third, false);
}

void Render::text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, const Color& color)
{
	interfaces::surface->drawTextFont(font);
	int width, height;

	interfaces::surface->getTextSize(font, text, width, height);
	interfaces::surface->setTextColor(color);
	interfaces::surface->drawTextPos(centered ? (x - (width / 2)) : x, y);
	interfaces::surface->drawRenderText(text, std::char_traits<wchar_t>::length(text));
}

void Render::text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color)
{
	if (text.empty())
		return;

	const auto converted = std::wstring(text.cbegin(), text.cend());
	int width, height;

	interfaces::surface->drawTextFont(font);
	interfaces::surface->getTextSize(font, converted.c_str(), width, height);
	interfaces::surface->setTextColor(color);
	interfaces::surface->drawTextPos(centered ? (x - (width / 2)) : x, y);
	interfaces::surface->drawRenderText(converted.c_str(), converted.size());
}

void Render::textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...)
{
	if (!fmt)
		return;

	if (strlen(fmt) < 2)
		return;

	va_list args;
	char buf[256];

	// this is dependable on CPU and project settings as with speed
	std::fill(buf, buf + 256, 0);

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	buf[sizeof(buf) - 1] = 0;
	va_end(args);

	text(x, y, font, buf, centered, color);
}

int Render::getTextSize(const unsigned long font, const std::string& text)
{
	std::wstring wtext(text.begin(), text.end());

	int width, height;
	interfaces::surface->getTextSize(font, wtext.c_str(), width, height);

	return width;
}

Vector2D Render::getTextSizeXY(const unsigned long font, const std::string& text)
{
	std::wstring wtext(text.begin(), text.end());

	int width, height;
	interfaces::surface->getTextSize(font, wtext.c_str(), width, height);

	return Vector2D(width, height);
}

#include "../../SDK/structs/IDXandPaterrns.hpp"
#include "../../cheats/globals.hpp"

bool Render::worldToScreen(const Vector& in, Vector& out)
{
	// this is correct way, engine itself is too bad and may be throwing bunch of random matrixes, by using this way you escape useless mess with dx render
	static auto addr = utilities::patternScan(CLIENT_DLL, VIEW_MATRIX_CLIENT);
	auto viewMatrix = *reinterpret_cast<uintptr_t*>(addr + 0x3) + 0xB0;

	const auto& screenMatrix = *reinterpret_cast<VMatrix*>(viewMatrix);

	float w = screenMatrix[3][0] * in.x + screenMatrix[3][1] * in.y + screenMatrix[3][2] * in.z + screenMatrix[3][3];

	if (w < 0.001f)
		return false;

	Vector2D viewport = { globals::screenX, globals::screenY };

	float inversed = 1.0f / w;
	out.x = (viewport.x / 2.0f) + (0.5f * ((screenMatrix[0][0] * in.x + screenMatrix[0][1] * in.y + screenMatrix[0][2] * in.z + screenMatrix[0][3]) * inversed) * viewport.x + 0.5f);
	out.y = (viewport.y / 2.0f) - (0.5f * ((screenMatrix[1][0] * in.x + screenMatrix[1][1] * in.y + screenMatrix[1][2] * in.z + screenMatrix[1][3]) * inversed) * viewport.y + 0.5f);
	out.z = 0.0f;

	return true;
}

// cs engine lines are not anti aliased :(

//#define CLASSIC_LINE
//#define DEBUG_RENDER
//#define TRIANGLE_METHOD
#define POLYGON_METHOD
	/*
	* Quick explanation, nobody explains this
	* Method drawline:
	* easy and very effective, every engine shares draw line function somewhere
	*
	* Method polygon
	* bit harder as not all engines shares such functions, so you must render it yourself with some lib / do it with dx/opgl
	* also notice normal rectangle will awlays fail there!
	*
	* Filling:
	* as polygon can be represented as two triangles there is no difference, just longer code

		   p4--------p5
		  /|		/|
		 / |	   / |
		p7-|------p6 |
		|  |      |  |
		|  |	  |  |
		|  |	  |	 |
		|  p0-----|-p1
		| /       | /
		|/        |/
		p3-------p2

	*/

void Render::drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled)
{
	constexpr size_t SIZE = 8;
	// transormed points to get pos.x/.y
	std::array<Vector, SIZE> points = {};

	for (size_t i = 0; i < box.size(); i++)
	{
		if (!worldToScreen(box.at(i), points.at(i)))
			return;
	}

	// anything with low alpha
	Color fill{ color.rMultiplied(), color.gMultiplied(), color.bMultiplied(), 30 };

	// lines to draw
	std::array<Vector2D, SIZE> lines = {};
	for (size_t i = 0; i < SIZE; i++)
	{
		lines.at(i) = { points.at(i).x, points.at(i).y };

#ifdef DEBUG_RENDER
		textf(lines.at(i).x, lines.at(i).y, fonts::tahoma, false, Color(100, 20, 100, 255), "[%i] posX: %0.2f, posY: %0.2f", i, lines.at(i).x, lines.at(i).y);
#endif // DEBUG_RENDER
	}
	// first fill then draw lines
	if (filled)
	{
#ifdef TRIANGLE_METHOD
		// bottom
		drawTriangle(lines.at(0), lines.at(1), lines.at(2), fill);
		drawTriangle(lines.at(2), lines.at(0), lines.at(3), fill);
		// top
		drawTriangle(lines.at(4), lines.at(5), lines.at(6), fill);
		drawTriangle(lines.at(6), lines.at(4), lines.at(7), fill);

		for (int i = 0; i < 3; i++)
		{
			drawTriangle(lines.at(i), lines.at(i + 1), lines.at(i + 4), fill);
			drawTriangle(lines.at(i + 4), lines.at(i + 5), lines.at(i + 1), fill);
		}
		// manually render left 
		drawTriangle(lines.at(3), lines.at(4), lines.at(7), fill);
		drawTriangle(lines.at(0), lines.at(3), lines.at(4), fill);
#endif // TRIANGLE_METHOD

#ifdef POLYGON_METHOD			 
		// bottom
		drawTrapezFilled(lines.at(0), lines.at(1), lines.at(2), lines.at(3), fill);
		// top
		drawTrapezFilled(lines.at(4), lines.at(5), lines.at(6), lines.at(7), fill);
		// front
		drawTrapezFilled(lines.at(3), lines.at(2), lines.at(6), lines.at(7), fill);
		// back
		drawTrapezFilled(lines.at(0), lines.at(1), lines.at(5), lines.at(4), fill);
		// right
		drawTrapezFilled(lines.at(2), lines.at(1), lines.at(5), lines.at(6), fill);
		// left
		drawTrapezFilled(lines.at(3), lines.at(0), lines.at(4), lines.at(7), fill);
#endif // POLYGON_METHOD
	}
#ifdef CLASSIC_LINE
	// bottom parts
	for (int i = 0; i < 3; i++)
	{
		drawLine(lines.at(i), lines.at(i + 1), color);
	}
	// missing part at the bottom
	drawLine(lines.at(0), lines.at(3), color);
	// top parts
	for (int i = 4; i < 7; i++)
	{
		drawLine(lines.at(i), lines.at(i + 1), color);
	}
	// missing part at the top
	drawLine(lines.at(4), lines.at(7), color);

	// now all 4 lines missing parts for 3d box
	for (int i = 0; i < 4; i++)
	{
		drawLine(lines.at(i), lines.at(i + 4), color);
	}
#endif // CLASSIC_LINE

#ifdef POLYGON_METHOD
	// bottom
	drawTrapezOutline(lines.at(0), lines.at(1), lines.at(2), lines.at(3), color);
	// top
	drawTrapezOutline(lines.at(4), lines.at(5), lines.at(6), lines.at(7), color);
	// front
	drawTrapezOutline(lines.at(3), lines.at(2), lines.at(6), lines.at(7), color);
	// back
	drawTrapezOutline(lines.at(0), lines.at(1), lines.at(5), lines.at(4), color);
	// right
	drawTrapezOutline(lines.at(2), lines.at(1), lines.at(5), lines.at(6), color);
	// left
	drawTrapezOutline(lines.at(3), lines.at(0), lines.at(4), lines.at(7), color);
#endif // POLYGON_METHOD
}

void Render::initNewTexture(int& id, Color* RGBA, const int w, const int h)
{
	id = interfaces::surface->createNewTextureID(true);
	if (id)
		interfaces::surface->setTextureRGBA(id, RGBA, w, h);
	else
		throw std::runtime_error(std::format(XOR("setTextureRGBA failed to create new texture, ID was: {}"), id));
}
void Render::initNewTexture(int& id, unsigned char* RGBA, const int w, const int h)
{
	id = interfaces::surface->createNewTextureID(true);
	if (id)
		interfaces::surface->setTextureRGBA(id, RGBA, w, h);
	else
		throw std::runtime_error(std::format(XOR("setTextureRGBA failed to create new texture, ID was: {}"), id));
}

void Render::drawFromTexture(const int id, const int x, const int y, const int w, const int h, const Color& color)
{
	if (!interfaces::surface->isTextureValid(id))
		return;

	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawSetTexture(id);
	interfaces::surface->drawTexturedRect(x, y, x + w, y + h);
}

void Render::drawProgressRing(const int x, const int y, float radius, const int points, float percent, const float thickness, const Color& color)
{
	// alfa means 0.0 - 1.0 range of how filled circle is
	percent = std::clamp(percent, 0.0f, 100.0f);
	float alfa = percent / 100.0f;

	// basically telling how precision will be
	float step = std::numbers::pi_v<float> *2.0f / points;

	// limit angle, based on percentage passed
	float maxAngle = std::numbers::pi_v<float> *2.0f * alfa;

	for (float angle = 0.0f; angle < maxAngle; angle += step)
	{
		float ax = x + (radius * std::cos(angle)); // - (std::numbers::pi_v<float> / 2.0f)) - will make clock like turning order
		float ay = y + (radius * std::sin(angle));

		float bx = x + (radius + thickness) * std::cos(angle);
		float by = y + (radius + thickness) * std::sin(angle);

		float cx = x + (radius * std::cos(angle + step));
		float cy = y + (radius * std::sin(angle + step));

		float dx = x + (radius + thickness) * std::cos(angle + step);
		float dy = y + (radius + thickness) * std::sin(angle + step);

		// aswell you can pass triangles with a bit higher precision in drawing

		// somethign brokey in this function, that's why this order
		drawTrapezFilled(
			Vector2D(cx, cy),
			Vector2D(dx, dy),
			Vector2D(bx, by),
			Vector2D(ax, ay),
			color);
	}
}

/*


		IMGUI DRAWING

*/

#include <ShlObj.h>
#include "../../dependencies/ImGui/imgui.h"

void ImGuiRender::init(ImGuiIO& io)
{
#ifdef _DEBUG
	if (CHAR fontsPath[MAX_PATH]; SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT, fontsPath)))
	{
		const std::filesystem::path path{ fontsPath };

		ImFontConfig cfg;
		ImFonts::tahoma = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("tahoma.ttf") }.string().c_str(), 14.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		ImFonts::espBar = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("framd.ttf") }.string().c_str(), 10.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		ImFonts::menuFont = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("Verdana.ttf") }.string().c_str(), 12.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		if (!ImGuiFreeType::BuildFontAtlas(io.Fonts))
			throw std::runtime_error(XOR("ImGuiFreeType::BuildFontAtlas returned false"));
	}
	else
		throw std::runtime_error(XOR("could not reach windows path"));
#else
	if (CHAR fontsPath[MAX_PATH]; SUCCEEDED(LF(SHGetFolderPathA).cached()(NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT, fontsPath)))
	{
		const std::filesystem::path path{ fontsPath };

		ImFontConfig cfg;
		ImFonts::tahoma = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("tahoma.ttf") }.string().c_str(), 14.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		ImFonts::espBar = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("framd.ttf") }.string().c_str(), 10.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		ImFonts::menuFont = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("Verdana.ttf") }.string().c_str(), 12.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		if (!ImGuiFreeType::BuildFontAtlas(io.Fonts))
			throw std::runtime_error(XOR("ImGuiFreeType::BuildFontAtlas returned false"));
	}
	else
		throw std::runtime_error(XOR("could not reach windows path"));
#endif

	LOG(LOG_INFO, XOR("init imgui fonts success"));
}

void ImGuiRender::drawLine(const float x, const float y, const float x2, const float y2, const Color& color, const float thickness)
{
	m_drawData.emplace_back(DrawType::LINE, std::make_any<LineObject_t>(LineObject_t(ImVec2{ x, y }, ImVec2{ x2, y2 }, U32(color), thickness)));
}

void ImGuiRender::drawLine(const Vector2D& start, const Vector2D& end, const Color& color, const float thickness)
{
	m_drawData.emplace_back(DrawType::LINE, std::make_any<LineObject_t>(LineObject_t(ImVec2{ start.x, start.y }, ImVec2{ end.x, end.y }, U32(color), thickness)));
}

void ImGuiRender::drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags, const float thickness)
{
	m_drawData.emplace_back(DrawType::RECT, std::make_any<RectObject_t>(RectObject_t(ImVec2{ x, y }, ImVec2{ x + w, y + h }, U32(color), 0.0f, flags, thickness)));
}

void ImGuiRender::drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags)
{
	m_drawData.emplace_back(DrawType::RECT_FILLED, std::make_any<RectObject_t>(RectObject_t(ImVec2{ x, y }, ImVec2{ x + w, y + h }, U32(color), 0.0f, flags)));
}

void ImGuiRender::drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags, const float thickness)
{
	m_drawData.emplace_back(DrawType::RECT, std::make_any<RectObject_t>(RectObject_t(ImVec2{ x, y }, ImVec2{ x + w, y + h }, U32(color), rounding, flags, thickness)));
}

void ImGuiRender::drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags)
{
	m_drawData.emplace_back(DrawType::RECT_FILLED, std::make_any<RectObject_t>(RectObject_t(ImVec2{ x, y }, ImVec2{ x + w, y + h }, U32(color), rounding, flags)));
}

void ImGuiRender::drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness)
{
	m_drawData.emplace_back(DrawType::CIRCLE, std::make_any<CircleObject_t>(CircleObject_t(ImVec2{ x, y }, radius, points, U32(color), thickness)));
}

void ImGuiRender::drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color)
{
	m_drawData.emplace_back(DrawType::CIRCLE_FILLED, std::make_any<CircleObject_t>(CircleObject_t(ImVec2{ x, y }, radius, points, U32(color))));
}

void ImGuiRender::drawCircle3D(const Vector& pos, const float radius, const int points, const Color& color, const ImDrawFlags flags, const float thickness)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector point(radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z);

		if (Vector screenStart; worldToScreen(point, screenStart))
			pointsVec.emplace_back(std::move(ImVec2{ screenStart.x, screenStart.y }));
	}

	m_drawData.emplace_back(DrawType::CIRCLE_3D, std::make_any<CircleObject_t>(CircleObject_t(pos, pointsVec, radius, points, U32(color), flags, thickness)));
}

void ImGuiRender::drawCircle3DTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const ImDrawFlags flags, const float thickness)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector start(radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z);

		Trace_t trace;
		TraceFilter filter;
		filter.m_skip = skip;

		interfaces::trace->traceRay({ pos, start }, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (Vector screenStart; worldToScreen(trace.m_end, screenStart))
			pointsVec.emplace_back(std::move(ImVec2{ screenStart.x, screenStart.y }));
	}

	m_drawData.emplace_back(DrawType::CIRCLE_3D, std::make_any<CircleObject_t>(CircleObject_t(pos, pointsVec, radius, points, U32(color), flags, thickness)));
}

void ImGuiRender::drawCircle3DFilled(const Vector& pos, const float radius, const int points, const Color& color, const Color& outline, const ImDrawFlags flags, const float thickness)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector point(radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z);

		if (Vector screenStart; imRender.worldToScreen(point, screenStart))
			pointsVec.emplace_back(std::move(ImVec2{ screenStart.x, screenStart.y }));
	}

	m_drawData.emplace_back(DrawType::CIRCLE_3D_FILLED, std::make_any<CircleObject_t>(CircleObject_t(pos, pointsVec, radius, points, U32(color), U32(outline), flags, thickness)));
}

void ImGuiRender::drawCircle3DFilledTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const Color& outline, const ImDrawFlags flags, const float thickness)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector start(radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z);

		Trace_t trace;
		TraceFilter filter;
		filter.m_skip = skip;

		interfaces::trace->traceRay({ pos, start }, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (Vector screenStart; worldToScreen(trace.m_end, screenStart))
			pointsVec.emplace_back(std::move(ImVec2{ screenStart.x, screenStart.y }));
	}

	m_drawData.emplace_back(DrawType::CIRCLE_3D_FILLED, std::make_any<CircleObject_t>(CircleObject_t(pos, pointsVec, radius, points, U32(color), U32(outline), flags, thickness)));
}

void ImGuiRender::drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color, const float thickness)
{
	m_drawData.emplace_back(DrawType::TRIANGLE, std::make_any<TriangleObject_t>(TriangleObject_t({ p1.x, p2.y }, { p2.x, p2.y }, { p3.x, p3.y }, U32(color), thickness)));
}

void ImGuiRender::drawTriangleFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color)
{
	m_drawData.emplace_back(DrawType::TRIANGLE_FILLED, std::make_any<TriangleObject_t>(TriangleObject_t({ p1.x, p2.y }, { p2.x, p2.y }, { p3.x, p3.y }, U32(color))));
}

void ImGuiRender::drawQuad(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color, const float thickness)
{
	m_drawData.emplace_back(DrawType::QUAD, std::make_any<QuadObject_t>(QuadObject_t({ p1.x, p1.y }, { p2.x, p2.y }, { p3.x, p3.y }, { p4.x, p4.y }, U32(color), thickness)));
}

void ImGuiRender::drawQuadFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
{
	m_drawData.emplace_back(DrawType::QUAD_FILLED, std::make_any<QuadObject_t>(QuadObject_t({ p1.x, p1.y }, { p2.x, p2.y }, { p3.x, p3.y }, { p4.x, p4.y }, U32(color))));
}

void ImGuiRender::drawPolyLine(const int count, ImVec2* verts, const Color& color, const ImDrawFlags flags, const float thickness)
{
	m_drawData.emplace_back(DrawType::POLYGON, std::make_any<PolygonObject_t>(PolygonObject_t(count, verts, U32(color), flags, thickness)));
}

void ImGuiRender::drawPolyGon(const int count, ImVec2* verts, const Color& color)
{
	m_drawData.emplace_back(DrawType::POLYGON_FILLED, std::make_any<PolygonObject_t>(PolygonObject_t(count, verts, U32(color))));
}

void ImGuiRender::drawGradient(const float x, const float y, const float w, const float h, const Color& first, const Color& second, bool horizontal)
{
	m_drawData.emplace_back(DrawType::RECT_GRADIENT, std::make_any<RectObject_t>(RectObject_t({ x, y }, { x + w, y + h }, U32(first), U32(second), horizontal)));
}

void ImGuiRender::text(const float x, const float y, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow)
{
	m_drawData.emplace_back(DrawType::TEXT, std::make_any<TextObject_t>(TextObject_t(font, { x, y }, U32(color), text, dropShadow, centered)));
}

void ImGuiRender::text(const float x, const float y, ImFont* font, const std::wstring& text, const bool centered, const Color& color, const bool dropShadow)
{
	std::string _text(text.length(), 0);
	// because warning
	std::transform(text.begin(), text.end(), _text.begin(), [](wchar_t wc)
		{
			return static_cast<char>(wc);
		});

	m_drawData.emplace_back(DrawType::TEXT, std::make_any<TextObject_t>(TextObject_t(font, { x, y }, U32(color), _text, dropShadow, centered)));
}

void ImGuiRender::textf(const float x, const float y, ImFont* font, const bool centered, const Color& color, const bool dropShadow, const char* fmt, ...)
{
	if (!fmt)
		return;

	if (strlen(fmt) < 2)
		return;

	va_list args;
	char buf[256];

	std::fill(buf, buf + 256, 0);

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	buf[sizeof(buf) - 1] = 0;
	va_end(args);

	text(x, y, font, buf, centered, color, dropShadow);
}

ImVec2 ImGuiRender::getTextSize(ImFont* font, const std::string& text)
{
	ImGui::PushFont(font);

	auto ret = ImGui::CalcTextSize(text.c_str());

	ImGui::PopFont();

	return ret;
}

bool ImGuiRender::worldToScreen(const Vector& in, Vector& out)
{
	static auto addr = utilities::patternScan(CLIENT_DLL, VIEW_MATRIX_CLIENT);
	auto viewMatrix = *reinterpret_cast<uintptr_t*>(addr + 0x3) + 0xB0;

	const auto& screenMatrix = *reinterpret_cast<VMatrix*>(viewMatrix);

	float w = screenMatrix[3][0] * in.x + screenMatrix[3][1] * in.y + screenMatrix[3][2] * in.z + screenMatrix[3][3];

	if (w < 0.001f)
		return false;

	ImVec2 viewport = ImGui::GetIO().DisplaySize;

	float inversed = 1.0f / w;
	out.x = (viewport.x / 2.0f) + (0.5f * ((screenMatrix[0][0] * in.x + screenMatrix[0][1] * in.y + screenMatrix[0][2] * in.z + screenMatrix[0][3]) * inversed) * viewport.x + 0.5f);
	out.y = (viewport.y / 2.0f) - (0.5f * ((screenMatrix[1][0] * in.x + screenMatrix[1][1] * in.y + screenMatrix[1][2] * in.z + screenMatrix[1][3]) * inversed) * viewport.y + 0.5f);
	out.z = 0.0f;

	return true;
}

bool ImGuiRender::worldToScreen(const Vector& in, Vector2D& out)
{
	static auto addr = utilities::patternScan(CLIENT_DLL, VIEW_MATRIX_CLIENT);
	auto viewMatrix = *reinterpret_cast<uintptr_t*>(addr + 0x3) + 0xB0;

	const auto& screenMatrix = *reinterpret_cast<VMatrix*>(viewMatrix);

	float w = screenMatrix[3][0] * in.x + screenMatrix[3][1] * in.y + screenMatrix[3][2] * in.z + screenMatrix[3][3];

	if (w < 0.001f)
		return false;

	ImVec2 viewport = ImGui::GetIO().DisplaySize;

	float inversed = 1.0f / w;
	out.x = (viewport.x / 2.0f) + (0.5f * ((screenMatrix[0][0] * in.x + screenMatrix[0][1] * in.y + screenMatrix[0][2] * in.z + screenMatrix[0][3]) * inversed) * viewport.x + 0.5f);
	out.y = (viewport.y / 2.0f) - (0.5f * ((screenMatrix[1][0] * in.x + screenMatrix[1][1] * in.y + screenMatrix[1][2] * in.z + screenMatrix[1][3]) * inversed) * viewport.y + 0.5f);

	return true;
}

void ImGuiRender::drawArc(const float x, const float y, float radius, const int points, float angleMin, float angleMax, const float thickness, const Color& color, const ImDrawFlags flags)
{
	m_drawData.emplace_back(DrawType::ARC, std::make_any<ArcObject_t>(ArcObject_t({ x, y }, radius, DEG2RAD(angleMin), DEG2RAD(angleMax), points, U32(color), flags, thickness)));
}

void ImGuiRender::drawProgressRing(const float x, const float y, const float radius, const int points, const float angleMin, float percent, const float thickness, const Color& color, const ImDrawFlags flags)
{
	percent = std::clamp(percent, 0.0f, 100.0f);
	float alfa = percent / 100.0f;

	float maxAngle = RAD2DEG(std::numbers::pi_v<float> *2.0f * alfa) + angleMin;

	m_drawData.emplace_back(DrawType::ARC, std::make_any<ArcObject_t>(ArcObject_t({ x, y }, radius, DEG2RAD(angleMin), DEG2RAD(maxAngle), points, U32(color), flags, thickness)));
}

void ImGuiRender::renderPresent(ImDrawList* draw)
{
	std::unique_lock<std::shared_mutex> lock{ m_mutex };

	if (m_drawDataSafe.empty())
		return;

	for (const auto& data : m_drawDataSafe)
	{
		if (!data.m_drawingObj.has_value())
			continue;

		switch (const auto val = data.m_drawingObj; data.m_type)
		{
		case DrawType::LINE:
		{
			const auto& obj = std::any_cast<LineObject_t>(val);
			draw->AddLine(obj.m_start, obj.m_end, obj.m_color, obj.m_thickness);
			break;
		}
		case DrawType::RECT:
		{
			const auto& obj = std::any_cast<RectObject_t>(val);
			draw->AddRect(obj.m_min, obj.m_max, obj.m_color1, obj.m_rounding, obj.m_flags, obj.m_thickness);
			break;
		}
		case DrawType::RECT_FILLED:
		{
			const auto& obj = std::any_cast<RectObject_t>(val);
			draw->AddRectFilled(obj.m_min, obj.m_max, obj.m_color1, obj.m_rounding, obj.m_flags);
			break;
		}
		case DrawType::RECT_GRADIENT: // FIXME: add possibility to draw multicolor gradient
		{
			const auto& obj = std::any_cast<RectObject_t>(val);

			if (!obj.m_horizontal)
				draw->AddRectFilledMultiColor(obj.m_min, obj.m_max, obj.m_color1, obj.m_color2, obj.m_color2, obj.m_color1);
			else
				draw->AddRectFilledMultiColor(obj.m_min, obj.m_max, obj.m_color1, obj.m_color1, obj.m_color2, obj.m_color2);

			break;
		}
		case DrawType::CIRCLE:
		{
			const auto& obj = std::any_cast<CircleObject_t>(val);
			draw->AddCircle(obj.m_centre, obj.m_radius, obj.m_color, obj.m_segments, obj.m_thickness);
			break;
		}
		case DrawType::CIRCLE_FILLED:
		{
			const auto& obj = std::any_cast<CircleObject_t>(val);
			draw->AddCircleFilled(obj.m_centre, obj.m_radius, obj.m_color, obj.m_segments);
			break;
		}
		case DrawType::CIRCLE_3D:
		{
			const auto& obj = std::any_cast<CircleObject_t>(val);
			draw->AddPolyline(obj.m_pointsVec.data(), obj.m_pointsVec.size(), obj.m_color, obj.m_flags, obj.m_thickness);
			break;
		}
		case DrawType::CIRCLE_3D_FILLED:
		{
			const auto& obj = std::any_cast<CircleObject_t>(val);
			draw->AddConvexPolyFilled(obj.m_pointsVec.data(), obj.m_pointsVec.size(), obj.m_color);
			draw->AddPolyline(obj.m_pointsVec.data(), obj.m_pointsVec.size(), obj.m_outline, obj.m_flags, obj.m_thickness);
			break;
		}
		case DrawType::TRIANGLE:
		{
			const auto& obj = std::any_cast<TriangleObject_t>(val);
			draw->AddTriangle(obj.m_p1, obj.m_p2, obj.m_p3, obj.m_color, obj.m_thickness);
			break;
		}
		case DrawType::TRIANGLE_FILLED:
		{
			const auto& obj = std::any_cast<TriangleObject_t>(val);
			draw->AddTriangleFilled(obj.m_p1, obj.m_p2, obj.m_p3, obj.m_color);
			break;
		}
		case DrawType::QUAD:
		{
			const auto& obj = std::any_cast<QuadObject_t>(val);
			draw->AddQuad(obj.m_p1, obj.m_p2, obj.m_p3, obj.m_p4, obj.m_color, obj.m_thickness);
			break;
		}
		case DrawType::QUAD_FILLED:
		{
			const auto& obj = std::any_cast<QuadObject_t>(val);
			draw->AddQuadFilled(obj.m_p1, obj.m_p2, obj.m_p3, obj.m_p4, obj.m_color);
			break;
		}
		case DrawType::POLYGON:
		{
			const auto& obj = std::any_cast<PolygonObject_t>(val);
			draw->AddPolyline(obj.m_verts, obj.m_count, obj.m_color, obj.m_flags, obj.m_thickness);
			break;
		}
		case DrawType::POLYGON_FILLED:
		{
			const auto& obj = std::any_cast<PolygonObject_t>(val);
			draw->AddConvexPolyFilled(obj.m_verts, obj.m_count, obj.m_color);
			break;
		}
		case DrawType::TEXT:
		{
			const auto& obj = std::any_cast<TextObject_t>(val);
			ImVec2 pos = { obj.m_pos.x, obj.m_pos.y };

			ImGui::PushFont(obj.m_font);

			if (auto tsize = ImGui::CalcTextSize(obj.m_text.c_str()); obj.m_centred)
				pos.x -= tsize.x / 2.0f;

			if (obj.m_dropShadow)
			{
				const auto alpha = ImGui::ColorConvertU32ToFloat4(obj.m_color).z;
				Color outline = Color(0.0f, 0.0f, 0.0f, alpha);
				draw->AddText({ pos.x + 1, pos.y + 1 }, U32(outline), obj.m_text.c_str());
			}
			draw->AddText(pos, obj.m_color, obj.m_text.c_str());

			ImGui::PopFont();

			break;
		}
		case DrawType::ARC:
		{
			const auto& obj = std::any_cast<ArcObject_t>(val);
			draw->PathArcTo(obj.m_centre, obj.m_radius, obj.m_aMax, obj.m_aMax, obj.m_segments);
			draw->PathStroke(obj.m_color, obj.m_flags, obj.m_thickness);
			break;
		}
		default:
			break;
		}
	}
}

void ImGuiRender::clearData()
{
	if (!m_drawData.empty())
		m_drawData.clear();
}

void ImGuiRender::swapData()
{
	std::unique_lock<std::shared_mutex> lock{ m_mutex };
	m_drawData.swap(m_drawDataSafe);
}

// DRAWING HELPER WRAPPER

#define RUNTIME_CHECK assert(m_drawing && "Did you call ImGuiRenderWindow::addList() ?")

void ImGuiRenderWindow::addList()
{
	m_drawing = ImGui::GetWindowDrawList();
	m_pos = ImGui::GetCursorScreenPos();
	auto limits = ImGui::GetContentRegionAvail(); // limits for width & height
	m_rect = { limits.x, limits.y };
}

void ImGuiRenderWindow::drawLine(const float x, const float y, const float x2, const float y2, const Color& color, const float thickness)
{
	RUNTIME_CHECK;
	m_drawing->AddLine({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x2, m_pos.y + y2 }, U32(color), thickness);
}

void ImGuiRenderWindow::drawLine(const Vector2D& start, const Vector2D& end, const Color& color, const float thickness)
{
	RUNTIME_CHECK;
	m_drawing->AddLine({ m_pos.x + start.x, m_pos.y + start.y }, { m_pos.x + end.x, m_pos.y + end.y }, U32(color), thickness);
}

void ImGuiRenderWindow::drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags, const float thickness)
{
	RUNTIME_CHECK;
	m_drawing->AddRect({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, U32(color), 0.0f, flags, thickness);
}

void ImGuiRenderWindow::drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags)
{
	RUNTIME_CHECK;
	m_drawing->AddRectFilled({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, U32(color), 0.0f, flags);
}

void ImGuiRenderWindow::drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags, const float thickness)
{
	RUNTIME_CHECK;
	m_drawing->AddRect({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, U32(color), rounding, flags, thickness);
}

void ImGuiRenderWindow::drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags)
{
	RUNTIME_CHECK;
	m_drawing->AddRectFilled({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, U32(color), rounding, flags);
}

void ImGuiRenderWindow::drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness)
{
	RUNTIME_CHECK;
	m_drawing->AddCircle({ m_pos.x + x, m_pos.y + y }, radius, U32(color), points, thickness);
}

void ImGuiRenderWindow::drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color)
{
	RUNTIME_CHECK;
	m_drawing->AddCircleFilled({ m_pos.x + x, m_pos.y + y }, radius, U32(color), points);
}