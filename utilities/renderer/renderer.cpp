#include "renderer.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

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

unsigned long render::__createFont(const char* fontName, const int size, const int weight, const unsigned long flags)
{
	auto result = interfaces::surface->fontCreate();
	interfaces::surface->setFontGlyph(result, fontName, size, weight, 0, 0, flags);
	return result;
}

void render::init()
{
	fonts::tahoma = __createFont(XOR("Tahoma"), 14, 800, FONTFLAG_OUTLINE);
	fonts::smalle = __createFont(XOR("Tahoma"), 9, 800, FONTFLAG_ANTIALIAS);
	fonts::espBar = __createFont(XOR("Franklin Gothic"), 10, 300, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts::menuFont = __createFont(XOR("Verdana"), 12, 350, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	LOG(LOG_INFO, XOR("render success"));
}

void render::drawLine(const int x, const int y, const int x2, const int y2, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawLine(x, y, x2, y2);
}

void render::drawLine(const Vector2D& start, const Vector2D& end, const Color& color)
{
	drawLine(start.x, start.y, end.x, end.y, color);
}

void render::drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawOutlinedRect(x, y, w, h);
}

void render::drawOutlineRect(const Vector2D& start, const Vector2D& end, const Color& color)
{
	drawOutlineRect(start.x, start.y, end.x, end.y, color);
}

void render::drawOutlineRect(const int x, const int y, const int w, const int h, const Color& color, const Color& colorOutline)
{
	drawOutlineRect(x, y, w, h, color);
	drawOutlineRect(x - 1, y - 1, w + 2, h + 2, colorOutline);
	drawOutlineRect(x + 1, y + 1, w - 2, h - 2, colorOutline);
}

void render::drawFilledRect(const int x, const int y, const int w, const int h, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawFilledRectangle(x, y, w, h);
}

void render::drawFilledRect(const int x, const int y, const int w, const int h, const Color& color, const Color& colorOutline)
{
	drawFilledRect(x, y, w, h, color);
	drawOutlineRect(x - 1, y - 1, w + 2, h + 2, colorOutline);
	drawOutlineRect(x + 1, y + 1, w - 2, h - 2, colorOutline);
}

void render::drawRoundedRect(const int x, const int y, const int w, const int h, const int radius, const int numberOfVertices, const Color& color)
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

void render::drawCircle(const int x, const int y, const int radius, const int points, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawOutlinedCircle(x, y, radius, points);
}

void render::drawCircleFilled(const int x, const int y, const int radius, const int points, const Color& color)
{
	std::vector<Vertex_t> verts = {};

	float step = std::numbers::pi_v<float> *2.0f / points;
	for (float i = 0; i < (std::numbers::pi_v<float> *2.0f); i += step)
	{
		verts.emplace_back(Vector2D(x + (radius * std::cos(i)), y + (radius * std::sin(i))));
	}
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawTexturedPolygon(verts.size(), verts.data());
}

void render::drawCircle3D(const Vector& pos, const int radius, const int points, const Color& color)
{
	float step = std::numbers::pi_v<float> *2.0f / points;

	for (float i = 0; i < (std::numbers::pi_v<float> *2.0f); i += step)
	{
		Vector start(radius * std::cos(i) + pos.x, radius * std::sin(i) + pos.y, pos.z);
		Vector end(radius * std::cos(i + step) + pos.x, radius * std::sin(i + step) + pos.y, pos.z);

		if (Vector screenStart, screenEnd; worldToScreen(start, screenStart) && worldToScreen(end, screenEnd))
		{
			drawLine(screenStart.x, screenStart.y, screenEnd.x, screenEnd.y, color);
		}
	}
}

void render::drawFilledCircle3D(const Vector& pos, const int radius, const int points, const Color& color)
{
	Vector orignalW2S = {};
	if (!worldToScreen(pos, orignalW2S))
		return;

	Vector screenStart = {};
	float step = std::numbers::pi_v<float> *2.0f / points;
	static Vector before = NOTHING;

	for (float i = 0; i < (std::numbers::pi_v<float> *2.0f); i += step)
	{
		Vector start(radius * std::cos(i) + pos.x, radius * std::sin(i) + pos.y, pos.z);

		if (worldToScreen(start, screenStart) && !before.IsZero())
		{
			drawLine(screenStart.x, screenStart.y, before.x, before.y, color);
			drawTriangle(Vector2D(orignalW2S.x, orignalW2S.y), Vector2D(screenStart.x, screenStart.y), Vector2D(before.x, before.y),
				Color(color.r(), color.g(), color.b(), color.a() / 4.0f));
		}
		before = screenStart;
	}
}


/*
		  + -p1
		 / \
		/   \
	   /	 \
 p2 - +-------+ - p3

 */

void render::drawTriangle(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Color& color)
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

/*	  p4+
		| `
		|	 `
		|		`
		|			`
		|			   `+p3
		|				|
		|				|
		|				|
		|				|
		|				|
	  p1+---------------+p2
 */

void render::drawTrapezFilled(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
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

void render::drawTrapezOutline(const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, const Vector2D& p4, const Color& color)
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

void render::drawPolyLine(int* x, int* y, const int count, const Color& color)
{
	interfaces::surface->drawSetColor(color);
	interfaces::surface->drawPolyLine(x, y, count);
}

void render::drawPolyLine(const int count, Vertex_t* verts, const Color& color)
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

void render::drawGradient(const int x, const int y, const int w, const int h, const Color& first, const Color& second, bool horizontal)
{
	auto gradient = [=](const Color& color, unsigned int alpha1, unsigned int alpha2)
	{
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawFilledFadeRect(x, y, w, h, alpha1, alpha2, horizontal);
	};

	if (!horizontal)
	{
		gradient(first, 255, 255);
		gradient(second, 0, 255);
	}
	else if (horizontal)
	{
		gradient(first, 255, 0);
		gradient(second, 0, 255);
	}
}

void render::text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, const Color& color)
{
	interfaces::surface->drawTextFont(font);
	int width, height;

	interfaces::surface->getTextSize(font, text, width, height);
	interfaces::surface->setTextColor(color);
	interfaces::surface->drawTextPos(centered ? (x - (width / 2)) : x, y);
	interfaces::surface->drawRenderText(text, wcslen(text));
}

void render::text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, const Color& color)
{
	if (text.empty())
		return;

	const auto converted = std::wstring(text.cbegin(), text.cend());
	int width, height;

	interfaces::surface->drawTextFont(font);
	interfaces::surface->getTextSize(font, converted.c_str(), width, height);
	interfaces::surface->setTextColor(color);
	interfaces::surface->drawTextPos(centered ? (x - (width / 2)) : x, y);
	interfaces::surface->drawRenderText(converted.c_str(), wcslen(converted.c_str()));
}

void render::textf(const int x, const int y, const unsigned long font, const bool centered, const Color& color, const char* fmt, ...)
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

int render::getTextSize(const unsigned long font, const std::string& text)
{
	std::wstring wtext(text.begin(), text.end());
	const wchar_t* wbuf = wtext.c_str();

	int width, height;
	interfaces::surface->getTextSize(font, wbuf, width, height);

	return width;
}

bool render::worldToScreen(const Vector& in, Vector& out)
{
	return interfaces::debugOverlay->worldToScreen(in, out) != 1;
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
		|  |      |	 |
		|  p0-----|-p1
		| /       | /
		|/        |/
		p3-------p2

	*/

void render::drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled)
{
	constexpr unsigned int SIZE = 8;
	// transormed points to get pos.x/.y
	std::array<Vector, SIZE> points = {};

	static bool check = true;
	for (size_t i = 0; i < box.size(); i++)
	{
		if (!worldToScreen(box.at(i), points.at(i)))
			check = false;
	}

	if (check)
	{
		// anything with low alpha
		Color fill{ color.r(), color.g(), color.b(), 30 };

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
		drawTrapezOutline(lines.at(0), lines.at(1), lines.at(2), lines.at(3), fill);
		// top
		drawTrapezOutline(lines.at(4), lines.at(5), lines.at(6), lines.at(7), fill);
		// front
		drawTrapezOutline(lines.at(3), lines.at(2), lines.at(6), lines.at(7), fill);
		// back
		drawTrapezOutline(lines.at(0), lines.at(1), lines.at(5), lines.at(4), fill);
		// right
		drawTrapezOutline(lines.at(2), lines.at(1), lines.at(5), lines.at(6), fill);
		// left
		drawTrapezOutline(lines.at(3), lines.at(0), lines.at(4), lines.at(7), fill);
#endif // POLYGON_METHOD
	}
}