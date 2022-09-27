#include "renderer.hpp"

#include "../math/math.hpp"
#include "../console/console.hpp"
#include "../tools/tools.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/ISurface.hpp>
#include <SDk/math/matrix.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/math/Vector2D.hpp>
#include <SDK/Color.hpp>
#include <utilities/res.hpp>

#include <ranges>

#define BUFFER_SIZE 256

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

unsigned long SurfaceRender::__createFont(const char* fontName, const int size, const int weight, const unsigned long flags)
{
	auto result = interfaces::surface->fontCreate();
	interfaces::surface->setFontGlyph(result, fontName, size, weight, 0, 0, flags);
	return result;
}

void SurfaceRender::init()
{
	fonts::tahoma = __createFont(XOR("Tahoma"), 14, 800, FONTFLAG_OUTLINE);
	fonts::franklinGothic = __createFont(XOR("Franklin Gothic"), 10, 300, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts::verdana = __createFont(XOR("Verdana"), 12, 350, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	console.log(TypeLogs::LOG_INFO, XOR("render init success"));
}

void SurfaceRender::drawLine(const int x, const int y, const int x2, const int y2, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawLine(x, y, x2, y2);
}

void SurfaceRender::drawLine(const Vector2D& start, const Vector2D& end, const Color& color)
{
	drawLine(static_cast<int>(start.x), static_cast<int>(start.y),
		static_cast<int>(end.x), static_cast<int>(end.y), color);
}

void SurfaceRender::drawRect(const int x, const int y, const int w, const int h, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawOutlinedRect(x, y, w, h);
}

void SurfaceRender::drawRect(const Vector2D& start, const Vector2D& end, const Color& color)
{
	drawRect(static_cast<int>(start.x), static_cast<int>(start.y),
		static_cast<int>(end.x), static_cast<int>(end.y), color);
}

void SurfaceRender::drawRectFilled(const int x, const int y, const int w, const int h, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawFilledRectangle(x, y, w, h);
}

void SurfaceRender::drawRoundedRect(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color)
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
			float rad = math::DEG2RAD((90.f * i) + (j / static_cast<float>(numberOfVertices - 1)) * 90.f);

			roundV[(i * numberOfVertices) + j] = Vertex_t{ Vector2D{_x + radius * std::sin(rad), _y - radius * std::cos(rad)} };
		}
	}

	drawPolyLine(4 * numberOfVertices, roundV.get(), color);
}

void SurfaceRender::drawRoundedRectFilled(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color)
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
			float rad = math::DEG2RAD((90.f * i) + (j / static_cast<float>(numberOfVertices - 1)) * 90.f);

			roundV[(i * numberOfVertices) + j] = Vertex_t{ Vector2D{_x + radius * std::sin(rad), _y - radius * std::cos(rad)} };
		}
	}

	drawPolyGon(4 * numberOfVertices, roundV.get(), color);
}

void SurfaceRender::drawCircle(const int x, const int y, const int radius, const int points, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawOutlinedCircle(x, y, radius, points);
}

void SurfaceRender::drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color)
{
	std::vector<Vertex_t> verts = {};

	float step = math::PI *2.0f / points;
	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		verts.emplace_back(Vector2D{ x + (radius * std::cos(angle)), y + (radius * std::sin(angle)) });
	}

	drawPolyGon(verts.size(), verts.data(), color);
}

void SurfaceRender::drawCircle3D(const Vector& pos, const int radius, const int points, const Color& color)
{
	float step = math::PI *2.0f / points;
	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector worldEnd = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (Vector2D start, end; worldToScreen(worldStart, start) && worldToScreen(worldEnd, end))
			drawLine(start, end, color);
	}
}

void SurfaceRender::drawFilledCircle3D(const Vector& pos, const int radius, const int points, const Color& color)
{
	Vector2D orignalW2S = {};
	if (!worldToScreen(pos, orignalW2S))
		return;

	float step = math::PI *2.0f / points;
	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector worldEnd = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (Vector2D start, end; worldToScreen(worldStart, start) && worldToScreen(worldEnd, end))
		{
			drawLine(start, end, color);
			drawTriangle(orignalW2S, start, end, Color(color.r(), color.g(), color.b(), color.a() / 4.0f));
		}
	}
}

void SurfaceRender::drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color)
{
	std::array verts =
	{
		Vertex_t{ p1 },
		Vertex_t{ p2 },
		Vertex_t{ p3 }
	};

	drawPolyLine(verts.size(), verts.data(), color);
}

void SurfaceRender::drawTriangleFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color)
{
	std::array verts =
	{
		Vertex_t{ p1 },
		Vertex_t{ p2 },
		Vertex_t{ p3 }
	};

	drawPolyGon(verts.size(), verts.data(), color);
}

void SurfaceRender::drawQuadFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
{
	std::array verts =
	{
		Vertex_t{ p1 },
		Vertex_t{ p2 },
		Vertex_t{ p3 },
		Vertex_t{ p4 }
	};

	drawPolyGon(verts.size(), verts.data(), color);
}

void SurfaceRender::drawQuad(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
{
	std::array verts =
	{
		Vertex_t{ p1 },
		Vertex_t{ p2 },
		Vertex_t{ p3 },
		Vertex_t{ p4 }
	};

	drawPolyLine(verts.size(), verts.data(), color);
}

void SurfaceRender::drawPolyLine(int* x, int* y, const int count, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawPolyLine(x, y, count);
}

void SurfaceRender::drawPolyLine(const int count, Vertex_t* verts, const Color& color)
{
	auto x = std::make_unique<int[]>(count);
	auto y = std::make_unique<int[]>(count);

	for (auto i : std::views::iota(0, count))
	{
		x[i] = static_cast<int>(verts[i].m_Position.x);
		y[i] = static_cast<int>(verts[i].m_Position.y);
	}

	drawPolyLine(x.get(), y.get(), count, color);
}

void SurfaceRender::drawPolyGon(const int count, Vertex_t* verts, const Color& color, const bool clipped)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawTexturedPolygon(count, verts, clipped);
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/189418-medical-attention-gradients-surface.html got fixed gradient blend from there
void SurfaceRender::drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal, bool blend)
{
	auto gradient = [=](const Color& clr, bool reversed)
	{
		interfaces::surface->drawSetColor(clr);
		interfaces::surface->drawFilledFadeRect(
			x, y, w, h,
			reversed ? clr.aMultiplied() : 0,
			reversed ? 0 : clr.aMultiplied(),
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
		drawRectFilled(x, y, w, h, blendColor(first, second, 0.5f));
	gradient(first, true);
	gradient(second, false);
}

void SurfaceRender::drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, const Color& third, bool horizontal, bool blend)
{
	auto gradient = [=](const Color& clr, bool reversed)
	{
		interfaces::surface->drawSetColor(clr);
		interfaces::surface->drawFilledFadeRect(
			x, y, w, h,
			reversed ? clr.aMultiplied() : 0,
			reversed ? 0 : clr.aMultiplied(),
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
		drawRectFilled(x, y, w, h, blendColor(first, second, third, 0.5f));
	gradient(first, true);
	gradient(second, false);
	gradient(third, false);
}

void SurfaceRender::text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, const Color& color)
{
	interfaces::surface->drawTextFont(font);
	int width, height;

	interfaces::surface->getTextSize(font, text, width, height);
	interfaces::surface->setTextColor(color);
	interfaces::surface->drawTextPos(centered ? (x - (width / 2)) : x, y);
	interfaces::surface->drawRenderText(text, std::char_traits<wchar_t>::length(text));
}

void SurfaceRender::text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color)
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

void SurfaceRender::textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...)
{
	if (!fmt)
		return;

	if (strlen(fmt) < 2)
		return;

	if (strlen(fmt) > BUFFER_SIZE)
		return; // maybe throw warn

	va_list args;

	std::array<char, BUFFER_SIZE> buf;

	std::fill(buf.begin(), buf.end(), 0);

	va_start(args, fmt);
	vsnprintf(buf.data(), sizeof(buf), fmt, args);
	buf[sizeof(buf) - 1] = 0;
	va_end(args);

	text(x, y, font, buf.data(), centered, color);
}

int SurfaceRender::getTextSize(const unsigned long font, const std::string& text)
{
	std::wstring wtext(text.begin(), text.end());

	int width, height;
	interfaces::surface->getTextSize(font, wtext.c_str(), width, height);

	return width;
}

Vector2D SurfaceRender::getTextSizeXY(const unsigned long font, const std::string& text)
{
	std::wstring wtext(text.begin(), text.end());

	int width, height;
	interfaces::surface->getTextSize(font, wtext.c_str(), width, height);

	return { (float)width, (float)height };
}

#include <gamememory/memory.hpp>
#include <game/globals.hpp>

bool SurfaceRender::worldToScreen(const Vector& in, Vector& out)
{
	auto screenMatrix = g_Memory.m_viewMatrixAddr();

	float w = screenMatrix[3][0] * in.x + screenMatrix[3][1] * in.y + screenMatrix[3][2] * in.z + screenMatrix[3][3];

	if (w < 0.001f)
		return false;

	Vector2D viewport = { (float)globals::screenX, (float)globals::screenY };

	float inversed = 1.0f / w;
	out.x = (viewport.x / 2.0f) + (0.5f * ((screenMatrix[0][0] * in.x + screenMatrix[0][1] * in.y + screenMatrix[0][2] * in.z + screenMatrix[0][3]) * inversed) * viewport.x + 0.5f);
	out.y = (viewport.y / 2.0f) - (0.5f * ((screenMatrix[1][0] * in.x + screenMatrix[1][1] * in.y + screenMatrix[1][2] * in.z + screenMatrix[1][3]) * inversed) * viewport.y + 0.5f);
	out.z = 0.0f;

	return true;
}

bool SurfaceRender::worldToScreen(const Vector& in, Vector2D& out)
{
	auto screenMatrix = g_Memory.m_viewMatrixAddr();

	float w = screenMatrix[3][0] * in.x + screenMatrix[3][1] * in.y + screenMatrix[3][2] * in.z + screenMatrix[3][3];

	if (w < 0.001f)
		return false;

	Vector2D viewport = { (float)globals::screenX, (float)globals::screenY };

	float inversed = 1.0f / w;
	out.x = (viewport.x / 2.0f) + (0.5f * ((screenMatrix[0][0] * in.x + screenMatrix[0][1] * in.y + screenMatrix[0][2] * in.z + screenMatrix[0][3]) * inversed) * viewport.x + 0.5f);
	out.y = (viewport.y / 2.0f) - (0.5f * ((screenMatrix[1][0] * in.x + screenMatrix[1][1] * in.y + screenMatrix[1][2] * in.z + screenMatrix[1][3]) * inversed) * viewport.y + 0.5f);

	return true;
}

// cs engine lines are not anti aliased :(

void SurfaceRender::drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled)
{
	// transormed points to get pos.x/.y
	std::array<Vector2D, 8> lines = {};

	for (size_t i = 0; auto& el : lines)
	{
		if (!worldToScreen(box.at(i), el))
			return;

#ifdef DEBUG_RENDER
		textf(points.at(i).x, points.at(i).y, fonts::tahoma, false, Color(100, 20, 100, 255), "[%i] posX: %0.2f, posY: %0.2f", i, points.at(i).x, points.at(i).y);
#endif // DEBUG_RENDER

		i++;
	}

	// anything with low alpha
	Color fill{ color.rMultiplied(), color.gMultiplied(), color.bMultiplied(), 30 };

	// first fill then draw lines
	if (filled)
	{
		// auto points = math::grahamScan(box.points); -> overload this function, I dont use surface for this
		// std::reverse(points.begin(), points.end()); -> don't use in surface.
		// drawPolyGon(points.size(), points.data(), fill);
	}
	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		drawLine(lines.at(i - 1), lines.at(i % 4), color);
		// TOP 4,5,6,7
		drawLine(lines.at(i + 3), lines.at(i % 4 + 4), color);
		// MISSING TOP
		drawLine(lines.at(i - 1), lines.at(i + 3), color);
	}
}

void SurfaceRender::initNewTexture(int& id, Color* RGBA, const int w, const int h)
{
	id = interfaces::surface->createNewTextureID(true);
	if (id)
		interfaces::surface->setTextureRGBA(id, RGBA, w, h);
	else
		throw std::runtime_error(FORMAT(XOR("setTextureRGBA failed to create new texture, ID was: {}"), id));
}
void SurfaceRender::initNewTexture(int& id, unsigned char* RGBA, const int w, const int h)
{
	id = interfaces::surface->createNewTextureID(true);
	if (id)
		interfaces::surface->setTextureRGBA(id, RGBA, w, h);
	else
		throw std::runtime_error(FORMAT(XOR("setTextureRGBA failed to create new texture, ID was: {}"), id));
}

void SurfaceRender::drawImage(const Resource& res, const int x, const int y, const int w, const int h, const Color& color)
{
	if (!interfaces::surface->isTextureValid(res.getTextureID()))
		return;

	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawSetTexture(res.getTextureID());
	interfaces::surface->drawTexturedRect(x, y, x + w, y + h);
}

void SurfaceRender::drawProgressRing(const int x, const int y, float radius, const int points, float percent, const float thickness, const Color& color)
{
	// basically telling how precision will be
	float step = math::PI *2.0f / points;

	// limit angle, based on percentage passed
	float maxAngle = math::PI *2.0f * percent;

	for (float angle = 0.0f; angle < maxAngle; angle += step)
	{
		float ax = x + (radius * std::cos(angle)); // - (math::PI / 2.0f)) - will make clock like turning order
		float ay = y + (radius * std::sin(angle));

		float bx = x + (radius + thickness) * std::cos(angle);
		float by = y + (radius + thickness) * std::sin(angle);

		float cx = x + (radius * std::cos(angle + step));
		float cy = y + (radius * std::sin(angle + step));

		float dx = x + (radius + thickness) * std::cos(angle + step);
		float dy = y + (radius + thickness) * std::sin(angle + step);

		// aswell you can pass triangles with a bit higher precision in drawing

		// somethign brokey in this function, that's why this order
		drawQuadFilled(
			Vector2D{ cx, cy },
			Vector2D{ dx, dy },
			Vector2D{ bx, by },
			Vector2D{ ax, ay },
			color);
	}
}

/*


		IMGUI DRAWING

*/

#include <ShlObj.h>
#include <filesystem>
#include <dependencies/ImGui/imgui_freetype.h>
#include "iconfont.hpp"

void ImGuiRender::init(ImGuiIO& io)
{
	if (CHAR fontsPath[MAX_PATH]; SUCCEEDED(LI_FN_CACHED(SHGetFolderPathA)(NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT, fontsPath)))
	{
		const std::filesystem::path path{ fontsPath };

		ImFontConfig cfg;
		cfg.OversampleH = 3;
		cfg.OversampleV = 3;
		cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;

		ImVector<ImWchar> range;
		ImFontGlyphRangesBuilder textBuilder;
		constexpr ImWchar textRanges[] =
		{
			0x0020, 0x00FF, // Basic Latin
			0x0100, 0x024F, // Latin Extended-A + Latin Extended-B
			0x0600, 0x06FF, // Arabic
			0x0E00, 0x0E7F, // Thai
			0
		};
		textBuilder.AddRanges(textRanges);
		textBuilder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
		textBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		textBuilder.BuildRanges(&range);

		ImFonts::tahoma14 = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("tahoma.ttf") }.string().c_str(), 14.0f, &cfg, textRanges);
		ImFonts::tahoma20 = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("tahoma.ttf") }.string().c_str(), 20.0f, &cfg, textRanges);
		ImFonts::franklinGothic12 = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("framd.ttf") }.string().c_str(), 12.0f, &cfg, textRanges);
		ImFonts::franklinGothic30 = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("framd.ttf") }.string().c_str(), 30.0f, &cfg, textRanges);
		ImFonts::verdana12 = io.Fonts->AddFontFromFileTTF(std::filesystem::path{ path / XOR("Verdana.ttf") }.string().c_str(), 12.0f, &cfg, textRanges);

		constexpr ImWchar ranges[] =
		{
			0xE000, 0xF8FF,
			0,
		};
		ImFonts::icon = io.Fonts->AddFontFromMemoryCompressedTTF(iconFont, iconFontSize, 80.0f, &cfg, ranges);

		if (!ImGuiFreeType::BuildFontAtlas(io.Fonts))
			throw std::runtime_error(XOR("ImGuiFreeType::BuildFontAtlas returned false"));
	}
	else
		throw std::runtime_error(XOR("could not reach windows path"));

	console.log(TypeLogs::LOG_INFO, XOR("init imgui fonts success"));
}

void ImGuiRender::drawLine(const float x, const float y, const float x2, const float y2, const Color& color, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Line>(ImVec2{ x, y }, ImVec2{ x2, y2 }, Color::U32(color), thickness));
}

void ImGuiRender::drawLine(const ImVec2& start, const ImVec2& end, const Color& color, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Line>(start, end, Color::U32(color), thickness));
}

void ImGuiRender::drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Rectangle>(ImVec2{ x, y }, ImVec2{ x + w, y + h }, Color::U32(color), 0.0f, flags, thickness));
}

void ImGuiRender::drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags)
{
	m_drawData.emplace_back(std::make_shared<drawing::RectangleFilled>(ImVec2{ x, y }, ImVec2{ x + w, y + h }, Color::U32(color), 0.0f, flags));
}

void ImGuiRender::drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Rectangle>(ImVec2{ x, y }, ImVec2{ x + w, y + h }, Color::U32(color), rounding, flags, thickness));
}

void ImGuiRender::drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags)
{
	m_drawData.emplace_back(std::make_shared<drawing::RectangleFilled>(ImVec2{ x, y }, ImVec2{ x + w, y + h }, Color::U32(color), rounding, flags));
}

void ImGuiRender::drawRectFilledMultiColor(const float x, const float y, const float w, const float h,
	const Color& colUprLeft, const Color& colUprRight, const Color& colBotRight, const Color& colBotLeft)
{
	m_drawData.emplace_back(std::make_shared<drawing::RectangleMultiColor>(ImVec2{ x, y }, ImVec2{ x + w, y + h },
		Color::U32(colUprLeft), Color::U32(colUprRight), Color::U32(colBotRight), Color::U32(colBotLeft)));
}

void ImGuiRender::drawTrianglePoly(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color)
{
	std::vector verts =
	{
		p1,
		p2,
		p3
	};

	imRender.drawPolyGon(verts, color);
}

static ImVec2 operator-(const ImVec2& v, float val)
{
	return ImVec2{ v.x - val, v.y - val };
}

static ImVec2 operator+(const ImVec2& v, float val)
{
	return ImVec2{ v.x + val, v.y + val };
}

void ImGuiRender::drawBox3D(const Vector& pos, const ImVec2& width, const float height, const Color& color, bool outlined, const float thickness)
{
	// dividing to get a centre to world position
	float boxW = width.x / 2.0f;
	float boxH = height;

	outlined = false; // looks bad

	float boxWidthSide = width.y / 2.0f;

	std::array box =
	{
		Vector{ pos.x - boxW, pos.y - boxWidthSide, pos.z },
		Vector{ pos.x - boxW, pos.y - boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y - boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y - boxWidthSide, pos.z },
		Vector{ pos.x - boxW, pos.y + boxWidthSide, pos.z },
		Vector{ pos.x - boxW, pos.y + boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y + boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y + boxWidthSide, pos.z },
	};

	std::array<ImVec2, box.size()> lines = {};
	
	for (size_t i = 0; auto& el : lines)
	{
		if (!worldToScreen(box.at(i), el))
			return;

		i++;
	}

	Color outlineCol = Colors::Black.getColorEditAlpha(color.a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		drawLine(lines.at(i - 1), lines.at(i % 4), color, thickness);
		if (outlined)
			drawLine(lines.at(i - 1) + 1.0f - thickness, lines.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		drawLine(lines.at(i + 3), lines.at(i % 4 + 4), color, thickness);
		if (outlined)
			drawLine(lines.at(i + 3) + 1.0f - thickness, lines.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		drawLine(lines.at(i - 1), lines.at(i + 3), color, thickness);
		if (outlined)
			drawLine(lines.at(i - 1) + 1.0f - thickness, lines.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

void ImGuiRender::drawBox3DFilled(const Vector& pos, const ImVec2& width, const float height, const Color& color, const Color& filling, bool outlined, const float thickness)
{
	// dividing to get a centre to world position
	float boxW = width.x / 2.0f;
	float boxH = height;

	outlined = false; // looks bad

	float boxWidthSide = width.y / 2.0f;

	std::array box =
	{
		Vector{ pos.x - boxW, pos.y - boxWidthSide, pos.z },
		Vector{ pos.x - boxW, pos.y - boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y - boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y - boxWidthSide, pos.z },
		Vector{ pos.x - boxW, pos.y + boxWidthSide, pos.z },
		Vector{ pos.x - boxW, pos.y + boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y + boxWidthSide, pos.z + boxH },
		Vector{ pos.x + boxW, pos.y + boxWidthSide, pos.z },
	};

	// transormed points to get pos.x/.y
	std::array<ImVec2, box.size()> lines = {};

	for (size_t i = 0; auto & el : lines)
	{
		if (!worldToScreen(box.at(i), el))
			return;

		i++;
	}

	auto points = math::grahamScan(lines);
	std::reverse(points.begin(), points.end());
	drawPolyGon(points, filling);

	Color outlineCol = Colors::Black.getColorEditAlpha(color.a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		drawLine(lines.at(i - 1), lines.at(i % 4), color, thickness);
		if (outlined)
			drawLine(lines.at(i - 1) + 1.0f - thickness, lines.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		drawLine(lines.at(i + 3), lines.at(i % 4 + 4), color, thickness);
		if (outlined)
			drawLine(lines.at(i + 3) + 1.0f - thickness, lines.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		drawLine(lines.at(i - 1), lines.at(i + 3), color, thickness);
		if (outlined)
			drawLine(lines.at(i - 1) + 1.0f - thickness, lines.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

void ImGuiRender::drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Circle>(ImVec2{ x, y }, radius, points, Color::U32(color), thickness));
}

void ImGuiRender::drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color)
{
	m_drawData.emplace_back(std::make_shared<drawing::CircleFilled>(ImVec2{ x, y }, radius, points, Color::U32(color)));
}

void ImGuiRender::drawCircle3D(const Vector& pos, const float radius, const int points, const Color& color, const ImDrawFlags flags, const float thickness)
{
	float step = math::PI * 2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (math::PI * 2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		if (ImVec2 screenStart; worldToScreen(worldStart, screenStart))
			pointsVec.push_back(screenStart);
	}

	drawPolyLine(pointsVec, color, flags, thickness);
}

#include <SDK/IEngineTrace.hpp>
#include <SDK/vars.hpp>

void ImGuiRender::drawCircle3DTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const ImDrawFlags flags, const float thickness)
{
	float step = math::PI *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };

		Trace_t trace;
		TraceFilter filter;
		filter.m_skip = skip;

		interfaces::trace->traceRay({ pos, worldStart }, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (ImVec2 screenStart; worldToScreen(trace.m_end, screenStart))
			pointsVec.push_back(screenStart);
	}

	drawPolyLine(pointsVec, color, flags, thickness);
}

void ImGuiRender::drawCircle3DFilled(const Vector& pos, const float radius, const int points, const Color& color, const Color& outline, const ImDrawFlags flags, const float thickness)
{
	float step = math::PI *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		if (ImVec2 screenStart; worldToScreen(worldStart, screenStart))
			pointsVec.push_back(screenStart);
	}

	drawPolyGon(pointsVec, color);
	drawPolyLine(pointsVec, color, flags, thickness);
}

void ImGuiRender::drawCircle3DFilledTraced(const Vector& pos, const float radius, const int points, void* skip, const Color& color, const Color& outline, const ImDrawFlags flags, const float thickness)
{
	float step = math::PI *2.0f / points;

	std::vector<ImVec2> pointsVec = {};
	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };

		Trace_t trace;
		TraceFilter filter;
		filter.m_skip = skip;

		interfaces::trace->traceRay({ pos, worldStart }, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (ImVec2 screenStart; worldToScreen(trace.m_end, screenStart))
			pointsVec.push_back(screenStart);
	}

	drawPolyGon(pointsVec, color);
	drawPolyLine(pointsVec, color, flags, thickness);
}

void ImGuiRender::drawTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Triangle>(p1, p2, p3, Color::U32(color), thickness));
}

void ImGuiRender::drawTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color)
{
	m_drawData.emplace_back(std::make_shared<drawing::TriangleFilled>(p1, p2, p3, Color::U32(color)));
}

void ImGuiRender::drawQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const Color& color, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Quad>(p1, p2, p3, p4, Color::U32(color), thickness));
}

void ImGuiRender::drawQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const Color& color)
{
	m_drawData.emplace_back(std::make_shared<drawing::QuadFilled>(p1, p2, p3, p4, Color::U32(color)));
}

void ImGuiRender::drawQuadFilledMultiColor(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4,
	const Color& colUprLeft, const Color& colUprRight, const Color& colBotRight, const Color& colBotLeft)
{
	m_drawData.emplace_back(std::make_shared<drawing::QuadMultiColor>(p1, p2, p3, p4,
		Color::U32(colUprLeft), Color::U32(colUprRight), Color::U32(colBotRight), Color::U32(colBotLeft)));
}

void ImGuiRender::drawPolyLine(const std::vector<ImVec2>& verts, const Color& color, const ImDrawFlags flags, const float thickness)
{
	m_drawData.emplace_back(std::make_shared<drawing::Polyline>(verts, Color::U32(color), flags, thickness));
}

void ImGuiRender::drawPolyGon(const std::vector<ImVec2>& verts, const Color& color)
{
	m_drawData.emplace_back(std::make_shared<drawing::Polygon>(verts, Color::U32(color)));
}

void ImGuiRender::drawPolyGonMultiColor(const std::vector<ImVec2>& verts, const std::vector<ImU32>& colors)
{
	m_drawData.emplace_back(std::make_shared<drawing::PolygonMultiColor>(verts, colors));
}

void ImGuiRender::drawGradient(const float x, const float y, const float w, const float h, const Color& first, const Color& second, bool horizontal)
{
	m_drawData.emplace_back(std::make_shared<drawing::RectangleGradient>(ImVec2{ x, y }, ImVec2{ x + w, y + h }, Color::U32(first), Color::U32(second), horizontal));
}

void ImGuiRender::text(const float x, const float y, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow)
{
	m_drawData.emplace_back(std::make_shared<drawing::Text>(font, ImVec2{ x, y }, Color::U32(color), text, dropShadow, centered));
}

void ImGuiRender::text(const float x, const float y, ImFont* font, const std::wstring& text, const bool centered, const Color& color, const bool dropShadow)
{
	std::string _text(text.length(), 0);
	// because warning
	std::transform(text.begin(), text.end(), _text.begin(), [](wchar_t wc)
		{
			return static_cast<char>(wc);
		});

	m_drawData.emplace_back(std::make_shared<drawing::Text>(font, ImVec2{ x, y }, Color::U32(color), _text, dropShadow, centered));
}

void ImGuiRender::text(const float x, const float y, const float fontSize, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow)
{
	m_drawData.emplace_back(std::make_shared<drawing::TextSize>(fontSize, font, ImVec2{ x, y }, Color::U32(color), text, dropShadow, centered));
}

void ImGuiRender::textf(const float x, const float y, ImFont* font, const bool centered, const Color& color, const bool dropShadow, const char* fmt, ...)
{
	if (!fmt)
		return;

	if (strlen(fmt) < 2)
		return;

	if (strlen(fmt) > BUFFER_SIZE)
		return; // maybe throw warn

	va_list args;

	std::array<char, BUFFER_SIZE> buf;

	std::fill(buf.begin(), buf.end(), 0);

	va_start(args, fmt);
	vsnprintf(buf.data(), sizeof(buf), fmt, args);
	buf[sizeof(buf) - 1] = 0;
	va_end(args);

	text(x, y, font, buf.data(), centered, color, dropShadow);
}

ImVec2 ImGuiRender::getTextSize(ImFont* font, const std::string& text)
{
	auto ret = ImGui::CalcTextSize(text.c_str());
	return ret;
}

bool ImGuiRender::worldToScreen(const Vector& in, Vector& out)
{
	auto screenMatrix = g_Memory.m_viewMatrixAddr();

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
	auto screenMatrix = g_Memory.m_viewMatrixAddr();

	float w = screenMatrix[3][0] * in.x + screenMatrix[3][1] * in.y + screenMatrix[3][2] * in.z + screenMatrix[3][3];

	if (w < 0.001f)
		return false;

	ImVec2 viewport = ImGui::GetIO().DisplaySize;

	float inversed = 1.0f / w;
	out.x = (viewport.x / 2.0f) + (0.5f * ((screenMatrix[0][0] * in.x + screenMatrix[0][1] * in.y + screenMatrix[0][2] * in.z + screenMatrix[0][3]) * inversed) * viewport.x + 0.5f);
	out.y = (viewport.y / 2.0f) - (0.5f * ((screenMatrix[1][0] * in.x + screenMatrix[1][1] * in.y + screenMatrix[1][2] * in.z + screenMatrix[1][3]) * inversed) * viewport.y + 0.5f);

	return true;
}

bool ImGuiRender::worldToScreen(const Vector& in, ImVec2& out)
{
	auto screenMatrix = g_Memory.m_viewMatrixAddr();

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
	m_drawData.emplace_back(std::make_shared<drawing::Arc>(ImVec2{ x, y }, radius, math::DEG2RAD(angleMin), math::DEG2RAD(angleMax), points, Color::U32(color), flags, thickness));
}

void ImGuiRender::drawProgressRing(const float x, const float y, const float radius, const int points, const float angleMin, float percent, const float thickness, const Color& color, const ImDrawFlags flags)
{
	float maxAngle = math::RAD2DEG(math::PI *2.0f * percent) + angleMin;
	m_drawData.emplace_back(std::make_shared<drawing::Arc>(ImVec2{ x, y }, radius, math::DEG2RAD(angleMin), math::DEG2RAD(maxAngle), points, Color::U32(color), flags, thickness));
}

void ImGuiRender::drawSphere(const Vector& pos, float radius, float angleSphere, const Color& color)
{
	std::vector<ImVec2> verts = {};

	float step = (1.0f / radius) + math::DEG2RAD(angleSphere);
	for (float angle = 0.0f; angle < math::PI; angle += step)
	{
		verts.clear();
		for (float angleBetween = 0.0f; angleBetween < math::PI * 2; angleBetween += step)
		{
			Vector worldStart =
			{
				radius * std::sin(angle) * std::cos(angleBetween) + pos.x,
				radius * std::sin(angle) * std::sin(angleBetween) + pos.y,
				radius * std::cos(angle) + pos.z
			};

			if (ImVec2 screenStart; imRender.worldToScreen(worldStart, screenStart))
				verts.push_back(screenStart);
		}

		if (verts.empty())
			continue;

		imRender.drawPolyLine(verts, color);
	}
}

void ImGuiRender::drawCone(const Vector& pos, const float radius, const int points, const float size, const Color& colCircle, const Color& colCone, const ImDrawFlags flags, const float thickness)
{
	ImVec2 orignalW2S = {};
	if (!worldToScreen(pos, orignalW2S))
		return;

	float step = math::PI *2.0f / points;

	for (float angle = 0.0f; angle < (math::PI *2.0f); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector worldEnd = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (ImVec2 start, end; worldToScreen(worldStart, start) && worldToScreen(worldEnd, end))
		{
			drawLine(start, end, colCircle);
			// using surface because it does not add outlines
			drawTrianglePoly({ orignalW2S.x, orignalW2S.y + size /*- std::abs(size)*/ }, start, end, colCone);
		}
	}
}

void ImGuiRender::renderPresent(ImDrawList* draw)
{
	std::unique_lock<std::shared_mutex> lock{ m_mutex };

	if (m_drawDataSafe.empty())
		return;

	for (auto data : m_drawDataSafe)
		data->draw(draw);	
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

void ImGuiRender::addToRender(const std::function<void()>& fun)
{
	clearData();
	fun();
	swapData();
}

// DRAWING HELPER WRAPPER

#define RUNTIME_CHECK_RENDER_WINDOW assert(m_drawing && "Did you call ImGuiRenderWindow::addList() ?")

void ImGuiRenderWindow::addList()
{
	m_drawing = ImGui::GetWindowDrawList();
	m_pos = ImGui::GetCursorScreenPos();
	auto limits = ImGui::GetContentRegionAvail(); // limits for width & height
	m_rect = { limits.x, limits.y };
}

void ImGuiRenderWindow::end()
{
	m_drawing = {};
	m_pos = {};
	m_rect = {};
}

void ImGuiRenderWindow::drawLine(const float x, const float y, const float x2, const float y2, const Color& color, const float thickness)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddLine({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x2, m_pos.y + y2 }, Color::U32(color), thickness);
}

void ImGuiRenderWindow::drawLine(const ImVec2& start, const ImVec2& end, const Color& color, const float thickness)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddLine({ m_pos.x + start.x, m_pos.y + start.y }, { m_pos.x + end.x, m_pos.y + end.y }, Color::U32(color), thickness);
}

void ImGuiRenderWindow::drawRect(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags, const float thickness)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddRect({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, Color::U32(color), 0.0f, flags, thickness);
}

void ImGuiRenderWindow::drawRectFilled(const float x, const float y, const float w, const float h, const Color& color, const ImDrawFlags flags)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddRectFilled({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, Color::U32(color), 0.0f, flags);
}

void ImGuiRenderWindow::drawRoundedRect(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags, const float thickness)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddRect({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, Color::U32(color), rounding, flags, thickness);
}

void ImGuiRenderWindow::drawRoundedRectFilled(const float x, const float y, const float w, const float h, const float rounding, const Color& color, const ImDrawFlags flags)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddRectFilled({ m_pos.x + x, m_pos.y + y }, { m_pos.x + x + w, m_pos.y + y + h }, Color::U32(color), rounding, flags);
}

void ImGuiRenderWindow::drawCircle(const float x, const float y, const float radius, const int points, const Color& color, const float thickness)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddCircle({ m_pos.x + x, m_pos.y + y }, radius, Color::U32(color), points, thickness);
}

void ImGuiRenderWindow::drawCircleFilled(const float x, const float y, const float radius, const int points, const Color& color)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddCircleFilled({ m_pos.x + x, m_pos.y + y }, radius, Color::U32(color), points);
}

void ImGuiRenderWindow::drawPolyLine(const int count, ImVec2* verts, const Color& color, ImDrawFlags flags, float thickness)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddPolyline(verts, count, Color::U32(color), flags, thickness);
}

void ImGuiRenderWindow::drawText(const float x, const float y, const float size, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	ImVec2 pos = { m_pos.x + x, m_pos.y + y };

	ImGui::PushFont(font);

	if (auto tsize = font->CalcTextSizeA(size, std::numeric_limits<float>::max(), 0.0f, text.c_str()); centered)
		pos.x -= tsize.x / 2.0f;

	if (dropShadow)
	{
		const auto alpha = ImGui::ColorConvertU32ToFloat4(Color::U32(color)).z;
		m_drawing->AddText(font, size, { m_pos.x + pos.x + 1.0f, m_pos.y + pos.y + 1.0f }, Color::U32(Colors::Black.getColorEditAlpha(alpha)), text.c_str());
	}
	m_drawing->AddText(font, size, pos, Color::U32(color), text.c_str());

	ImGui::PopFont();
}

void ImGuiRenderWindow::drawTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const Color& color)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	m_drawing->AddTriangleFilled({ m_pos.x + p1.x,m_pos.y + p1.y }, { m_pos.x + p2.x, m_pos.y + p2.y }, { m_pos.x + p3.x, m_pos.y + p3.y }, Color::U32(color));
}

void ImGuiRenderWindow::drawProgressRing(const float x, const float y, const float radius, const int points, const float angleMin, float percent, const float thickness, const Color& color, const ImDrawFlags flags)
{
	RUNTIME_CHECK_RENDER_WINDOW;
	float maxAngle = math::RAD2DEG(math::PI *2.0f * percent) + angleMin;

	m_drawing->PathArcTo(ImVec2{m_pos.x + x, m_pos.y + y }, radius, math::DEG2RAD(angleMin), math::DEG2RAD(maxAngle), points);
	m_drawing->PathStroke(Color::U32(color), flags, thickness);
}

#undef RUNTIME_CHECK_RENDER_WINDOW
#undef BUFFER_SIZE