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

unsigned long fonts::tahoma;
unsigned long fonts::smalle;

namespace render
{
	void init()
	{
		fonts::tahoma = interfaces::surface->fontCreate();
		interfaces::surface->setFontGlyph(fonts::tahoma, "Tahoma", 14, 800, 0, 0, FONTFLAG_OUTLINE);

		fonts::smalle = interfaces::surface->fontCreate();
		interfaces::surface->setFontGlyph(fonts::smalle, "Tahoma", 7, 800, 0, 0, FONTFLAG_OUTLINE);
		CONSOLE_INFO();
		LOG("[init] render success\n");
		CONSOLE_RESET();
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

		if (centered)
		{
			interfaces::surface->getTextSize(font, text, width, height);
			interfaces::surface->drawTextPos(x - width / 2, y);
		}
		else
			interfaces::surface->drawTextPos(x, y);

		interfaces::surface->setTextColor(color);
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
		interfaces::surface->drawTextPos((centered) ? (x - width / 2, y) : x, y);
		interfaces::surface->drawRenderText(converted.c_str(), wcslen(converted.c_str()));

	}

	void textf(const int x, const int y, const unsigned long font, const bool centered, Color color, const char* fmt, ...)
	{
		if (!fmt)
			return;

		if (strlen(fmt) < 2)
			return;

		va_list args;
		char buf[256] = {};

		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);

		wchar_t wbuf[256];
		MultiByteToWideChar(CP_UTF8, 0, buf, IM_ARRAYSIZE(wbuf), wbuf, IM_ARRAYSIZE(wbuf));

		int width, height;
		interfaces::surface->getTextSize(font, wbuf, width, height);

		if (centered)
		{
			interfaces::surface->getTextSize(font, wbuf, width, height);
			interfaces::surface->drawTextPos(x - width / 2, y);
		}
		else
			interfaces::surface->drawTextPos(x, y);

		interfaces::surface->drawTextFont(font);
		interfaces::surface->setTextColor(color);
		interfaces::surface->drawRenderText(wbuf, wcslen(wbuf));
	}

	int getTextSize(const unsigned long font, const char* fmt, ...)
	{
		va_list va_alist;
		char buf[256] = {};
		va_start(va_alist, fmt);
		_vsnprintf(buf, sizeof(buf), fmt, va_alist);
		va_end(va_alist);

		wchar_t wbuf[256] = {};
		MultiByteToWideChar(CP_UTF8, 0, buf, IM_ARRAYSIZE(buf), wbuf, IM_ARRAYSIZE(wbuf));

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

	void drawBox3D(Vector* box, const Color& color, bool filled)
	{
		const unsigned int SIZE = 8;
		// transormed points to get pos.x/.y
		Vector points[SIZE] = {};

		if (WorldToScreen(box[0], points[0]) &&
			WorldToScreen(box[1], points[1]) &&
			WorldToScreen(box[2], points[2]) &&
			WorldToScreen(box[3], points[3]) &&
			WorldToScreen(box[4], points[4]) &&
			WorldToScreen(box[5], points[5]) &&
			WorldToScreen(box[6], points[6]) &&
			WorldToScreen(box[7], points[7]))
		{
			// anything with low alpha
			Color fill{ color.r(), color.g(), color.b(), 30 };

			// lines to draw
			Vector2D lines[SIZE] = {};
			for (int i = 0; i < SIZE; i++)
			{
				lines[i] = { points[i].x, points[i].y };

#ifdef DEBUG_RENDER
				textf(lines[i].x, lines[i].y, fonts::tahoma, false, Color(100, 20, 100, 255), "[%i] posX: %0.2f, posY: %0.2f", i, lines[i].x, lines[i].y);
#endif // DEBUG_RENDER
			}
			// first fill then draw lines
			if (filled)
			{
#ifdef TRIANGLE_METHOD
				// bottom
				drawTriangle(lines[0], lines[1], lines[2], fill);
				drawTriangle(lines[2], lines[0], lines[3], fill);
				// top
				drawTriangle(lines[4], lines[5], lines[6], fill);
				drawTriangle(lines[6], lines[4], lines[7], fill);

				for (int i = 0; i < 3; i++)
				{
					drawTriangle(lines[i], lines[i + 1], lines[i + 4], fill);
					drawTriangle(lines[i + 4], lines[i + 5], lines[i + 1], fill);
				}
				// manually render left 
				drawTriangle(lines[3], lines[4], lines[7], fill);
				drawTriangle(lines[0], lines[3], lines[4], fill);
#endif // TRIANGLE_METHOD

#ifdef POLYGON_METHOD			 
				// bottom
				drawTrapezFilled(lines[0], lines[1], lines[2], lines[3], fill);
				// top
				drawTrapezFilled(lines[4], lines[5], lines[6], lines[7], fill);
				// front
				drawTrapezFilled(lines[3], lines[2], lines[6], lines[7], fill);
				// back
				drawTrapezFilled(lines[0], lines[1], lines[5], lines[4], fill);
				// right
				drawTrapezFilled(lines[2], lines[1], lines[5], lines[6], fill);
				// left
				drawTrapezFilled(lines[3], lines[0], lines[4], lines[7], fill);
#endif // POLYGON_METHOD
			}
#ifdef CLASSIC_LINE
			// bottom parts
			for (int i = 0; i < 3; i++)
			{
				drawLine(lines[i], lines[i + 1], color);
			}
			// missing part at the bottom
			drawLine(lines[0], lines[3], color);
			// top parts
			for (int i = 4; i < 7; i++)
			{
				drawLine(lines[i], lines[i + 1], color);
			}
			// missing part at the top
			drawLine(lines[4], lines[7], color);

			// now all 4 lines missing parts for 3d box
			for (int i = 0; i < 4; i++)
			{
				drawLine(lines[i], lines[i + 4], color);
			}
#endif // CLASSIC_LINE

#ifdef POLYGON_METHOD
			// bottom
			drawTrapezOutline(lines[0], lines[1], lines[2], lines[3], color);
			// top
			drawTrapezOutline(lines[4], lines[5], lines[6], lines[7], color);
			// front
			drawTrapezOutline(lines[3], lines[2], lines[6], lines[7], color);
			// back
			drawTrapezOutline(lines[0], lines[1], lines[5], lines[4], color);
			// right
			drawTrapezOutline(lines[2], lines[1], lines[5], lines[6], color);
			// left
			drawTrapezOutline(lines[3], lines[0], lines[4], lines[7], color);
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