#pragma once

#include <vector>
#include <string>

struct ImDrawList;
struct ImVec2;
struct ImFont;

// raw drawing
namespace drawing
{
	// base class, no fields. Use for any geometric structure.
	class Draw
	{
	public:
		virtual void draw(ImDrawList* draw) const = 0;
	};

	class Line : public Draw
	{
	public:
		constexpr Line(const ImVec2& min, const ImVec2& max, ImU32 color, float thickness)
			: m_start{ min }, m_end{ max }, m_color{ color }, m_thickness{ thickness }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_start;
		ImVec2 m_end;
		ImU32 m_color;
		float m_thickness;
	};

	class Rectangle : public Draw
	{
	public:
		constexpr Rectangle(const ImVec2& min, const ImVec2& max, ImU32 color, float rounding, ImDrawFlags flags, float thickness)
			: m_min{ min }, m_max{ max }, m_color{ color }, m_rounding{ rounding }, m_flags{ flags }, m_thickness{ thickness }
		{}
		
		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_min;
		ImVec2 m_max;
		ImU32 m_color;
		ImDrawFlags m_flags;
		float m_thickness;
		float m_rounding;
	};

	class RectangleFilled : public Draw
	{
	public:
		constexpr RectangleFilled(const ImVec2& min, const ImVec2& max, ImU32 color, float rounding, ImDrawFlags flags)
			: m_min{ min }, m_max{ max }, m_color{ color }, m_rounding{ rounding }, m_flags{ flags }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_min;
		ImVec2 m_max;
		ImU32 m_color;
		float m_rounding;
		ImDrawFlags m_flags;
	};

	class RectangleGradient : public Draw
	{
	public:
		constexpr RectangleGradient(const ImVec2& min, const ImVec2& max, ImU32 color1, ImU32 color2, bool horizontal)
			: m_min{ min }, m_max{ max }, m_color1{ color1 }, m_color2{ color2 }, m_horizontal{ horizontal }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_min;
		ImVec2 m_max;
		ImU32 m_color1; // left up / normal color
		ImU32 m_color2; // right up
		ImDrawFlags m_flags;
		bool m_horizontal;
	};

	class RectangleMultiColor : public Draw
	{
	public:
		constexpr RectangleMultiColor(const ImVec2& min, const ImVec2& max, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4)
			: m_min{ min }, m_max{ max }, m_color1{ color1 }, m_color2{ color2 }, m_color3{ color3 }, m_color4{ color4 }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_min;
		ImVec2 m_max;
		ImU32 m_color1; // left up / normal color
		ImU32 m_color2; // right up
		ImU32 m_color3; // right bottom
		ImU32 m_color4; // left bottom
		ImDrawFlags m_flags;
		bool m_horizontal;
	};

	class Circle : public Draw
	{
	public:
		constexpr Circle(const ImVec2& centre, float radius, int segments, ImU32 color, float thickness)
			: m_centre{ centre }, m_radius{ radius }, m_segments{ segments }, m_color{ color }, m_thickness{ thickness }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_centre;
		float m_radius;
		ImU32 m_color;
		int m_segments;
		float m_thickness;
	};

	class CircleFilled : public Draw
	{
	public:
		constexpr CircleFilled(const ImVec2& centre, float radius, int segments, ImU32 color)
			: m_centre{ centre }, m_radius{ radius }, m_segments{ segments }, m_color{ color }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_centre;
		float m_radius;
		ImU32 m_color;
		int m_segments;
	};

	class Triangle : public Draw
	{
	public:
		constexpr Triangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 color, float thickness)
			: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_color{ color }, m_thickness{ thickness }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_p1;
		ImVec2 m_p2;
		ImVec2 m_p3;
		ImU32 m_color;
		float m_thickness;
	};

	class TriangleFilled : public Draw
	{
	public:
		constexpr TriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 color)
			: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_color{ color }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_p1;
		ImVec2 m_p2;
		ImVec2 m_p3;
		ImU32 m_color;
	};

	class Quad : public Draw
	{
	public:
		constexpr Quad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color, float thickness)
			: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_p4{ p4 }, m_color{ color }, m_thickness{ thickness }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_p1;
		ImVec2 m_p2;
		ImVec2 m_p3;
		ImVec2 m_p4;
		ImU32 m_color;
		float m_thickness;
	};

	class QuadFilled : public Draw
	{
	public:
		constexpr QuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color)
			: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_p4{ p4 }, m_color{ color }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_p1;
		ImVec2 m_p2;
		ImVec2 m_p3;
		ImVec2 m_p4;
		ImU32 m_color;
	};

	class QuadMultiColor : public Draw
	{
	public:
		constexpr QuadMultiColor(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4)
			: m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }, m_p4{ p4 }, m_color1{ color1 }, m_color2{ color2 }, m_color3{ color3 }, m_color4{ color4 }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_p1;
		ImVec2 m_p2;
		ImVec2 m_p3;
		ImVec2 m_p4;
		ImU32 m_color1; // left up / normal color
		ImU32 m_color2; // right up
		ImU32 m_color3; // right bottom
		ImU32 m_color4; // left bottom
	};

	class Polyline : public Draw
	{
	public:
		constexpr Polyline(const std::vector<ImVec2>& verts, ImU32 color, ImDrawFlags flags, float thickness)
			: m_count{ verts.size() }, m_verts{ verts }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		size_t m_count;
		std::vector<ImVec2> m_verts;
		ImU32 m_color;
		ImDrawFlags m_flags;
		float m_thickness;
	};

	class Polygon : public Draw
	{
	public:
		constexpr Polygon(const std::vector<ImVec2>& verts, ImU32 color)
			: m_count{ verts.size() }, m_verts{ verts }, m_color{ color }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		size_t m_count;
		std::vector<ImVec2> m_verts;
		ImU32 m_color;
	};

	class PolygonMultiColor : public Draw
	{
	public:
		constexpr PolygonMultiColor(const std::vector<ImVec2>& verts, const std::vector<ImU32>& colors)
			: m_count{ verts.size() }, m_verts{ verts }, m_colors{ colors }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		size_t m_count;
		std::vector<ImVec2> m_verts;
		std::vector<ImU32> m_colors;
	};

	class Text : public Draw
	{
	public:
		constexpr Text(ImFont* font, const ImVec2& pos, ImU32 color, const std::string& text, bool dropShadow, bool centered)
			: m_font{ font }, m_pos{ pos }, m_color{ color }, m_text{ text }, m_dropShadow{ dropShadow }, m_centred{ centered }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_pos;
		ImFont* m_font;
		std::string m_text;
		ImU32 m_color;
		bool m_dropShadow;
		bool m_centred;
	};

	class TextSize : public Draw
	{
	public:
		constexpr TextSize(float fontSize, ImFont* font, const ImVec2& pos, ImU32 color, const std::string& text, bool dropShadow, bool centered)
			: m_size{ fontSize }, m_font{ font }, m_pos{ pos }, m_color{ color }, m_text{ text }, m_dropShadow{ dropShadow }, m_centred{ centered }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_pos;
		ImFont* m_font;
		std::string m_text;
		ImU32 m_color;
		bool m_dropShadow;
		bool m_centred;
		float m_size;
	};

	class Arc : public Draw
	{
	public:
		constexpr Arc(const ImVec2& centre, float radius, float aMin, float aMax, int segments, ImU32 color, ImDrawFlags flags, float thickness)
			: m_centre{ centre }, m_radius{ radius }, m_aMin{ aMin }, m_aMax{ aMax }, m_segments{ segments }, m_color{ color }, m_flags{ flags }, m_thickness{ thickness }
		{}

		virtual void draw(ImDrawList* draw) const override;
	private:
		ImVec2 m_centre;
		float m_radius;
		float m_aMin;
		float m_aMax;
		int m_segments;
		ImU32 m_color;
		ImDrawFlags m_flags;
		float m_thickness;
	};
}