#include "renderer.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#define STRING2(x) #x
#define STRING(x) STRING2(x)

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

unsigned long fonts::tahoma;
unsigned long fonts::smalle;

namespace render
{
	void init()
	{
		fonts::tahoma = interfaces::surface->fontCreate();
		interfaces::surface->setFontGlyph(fonts::tahoma, "Tahoma", 14, 800, 0, 0, FONTFLAG_OUTLINE);

		fonts::smalle = interfaces::surface->fontCreate();
		interfaces::surface->setFontGlyph(fonts::smalle, "Tahoma", 9, 800, 0, 0, FONTFLAG_ANTIALIAS);
		LOG(LOG_INFO, "render success\n");
	}

	void drawLine(const int x, const int y, const int x2, const int y2, Color color)
	{
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawLine(x, y, x2, y2);
	}

	void drawLine(Vector2D start, Vector2D end, Color color)
	{
		drawLine(start.x, start.y, end.x, end.y, color);
	}

	void drawOutlineRect(const int x, const int y, const int w, const int h, Color color)
	{
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawOutlinedRect(x, y, w, h);
	}

	void drawOutlineRect(Vector2D start, Vector2D end, Color color)
	{
		drawOutlineRect(start.x, start.y, end.x, end.y, color);
	}

	void drawFilledRect(const int x, const int y, const int w, const int h, Color color)
	{
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawFilledRectangle(x, y, w, h);
	}

	void drawCircle(const int x, const int y, const int radius, const int points, Color color)
	{
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawOutlinedCircle(x, y, radius, points);
	}

	void drawCircleFilled(const int x, const int y, const int radius, const int points, Color color)
	{
		std::vector<Vertex_t> verts{};

		float step = M_PI * 2.0 / points;
		for (float i = 0; i < (M_PI * 2.0); i += step)
		{
			verts.emplace_back(Vector2D(x + (radius * std::cos(i)), y + (radius * std::sin(i))));
		}
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawTexturedPolygon(verts.size(), verts.data());
	}

	/*
			  + -p1
			 / \
			/   \
		   /	 \
	 p2 - +-------+ - p3

	 */

	void drawTriangle(Vector2D p1, Vector2D p2, Vector2D p3, Color color)
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

	void drawTrapezFilled(Vector2D p1, Vector2D p2, Vector2D p3, Vector2D p4, Color color)
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

	void drawTrapezOutline(Vector2D p1, Vector2D p2, Vector2D p3, Vector2D p4, Color color)
	{
		Vertex_t verts[] =
		{
			Vertex_t(p1),
			Vertex_t(p2),
			Vertex_t(p3),
			Vertex_t(p4)
		};

		interfaces::surface->drawSetColor(color);
		drawPolygonOutline(4, verts, color);
	}

	void drawPolyLine(int* x, int* y, const int count, Color color)
	{
		interfaces::surface->drawSetColor(color);
		interfaces::surface->drawPolyLine(x, y, count);
	}

	void drawPolyLine(const int count, Vertex_t* verts, Color color)
	{
		static int x[128];
		static int y[128];

		for (int i = 0; i < count; i++)
		{
			x[i] = verts[i].m_Position.x;
			y[i] = verts[i].m_Position.y;
		}

		drawPolyLine(x, y, count, color);
	}

	void drawPolygonOutline(const int count, Vertex_t* verts, Color color)
	{
		static int x[128];
		static int y[128];

		for (int i = 0; i < count; i++)
		{
			x[i] = std::ceil(verts[i].m_Position.x);
			y[i] = std::ceil(verts[i].m_Position.y);
		}

		drawPolyLine(x, y, count, color);
	}

	void text(const int x, const int y, const unsigned long font, const wchar_t* text, const bool centered, Color color)
	{
		interfaces::surface->drawTextFont(font);
		int width, height;

		interfaces::surface->getTextSize(font, text, width, height);
		interfaces::surface->setTextColor(color);
		interfaces::surface->drawTextPos(centered ? (x - (width / 2)) : x, y);
		interfaces::surface->drawRenderText(text, wcslen(text));
	}

	void text(const int x, const int y, const unsigned long font, const std::string& text, const bool centered, Color color)
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

	void textf(const int x, const int y, const unsigned long font, const bool centered, Color color, const char* fmt, ...)
	{
#pragma message ("use render::text with std::format rather than render::textf if possible, line: " STRING(__LINE__) " in: " __FUNCTION__)

		if (!fmt)
			return;

		if (strlen(fmt) < 2)
			return;

		va_list args;
		char buf[256] = {};

		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		buf[sizeof(buf) - 1] = 0;
		va_end(args);

		text(x, y, font, buf, centered, color);
	}

	int getTextSize(const unsigned long font, const std::string& text)
	{
		std::wstring wtext(text.begin(), text.end());
		const wchar_t* wbuf = wtext.c_str();

		int width, height;
		interfaces::surface->getTextSize(font, wbuf, width, height);

		return width;
	}

	bool WorldToScreen(const Vector& in, Vector& out)
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

	void drawBox3D(const std::array<Vector, 8>& box, const Color& color, bool filled)
	{
		constexpr unsigned int SIZE = 8;
		// transormed points to get pos.x/.y
		std::array<Vector, SIZE> points = {};

		static bool check = true;
		for (size_t i = 0; i < box.size(); i++)
		{
			if(!WorldToScreen(box.at(i), points.at(i)))
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

	/*RECT viewport()
	{
		RECT viewport = {};

		static int width, height;
		interfaces::engine->getScreenSize(width, height);
		viewport.right = width;
		viewport.bottom = height;

		return viewport;
	}*/
}