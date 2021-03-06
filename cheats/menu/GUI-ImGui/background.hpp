#pragma once

#include <vector>

#include "../../../SDK/Color.hpp"
#include "../../../SDK/math/Vector2D.hpp"

struct ImDrawList;

struct ParticlePoint_t
{
	constexpr ParticlePoint_t(const Vector2D& pos, const Vector2D& move, const Color& color) :
		m_pos{ pos }, m_move{ move }, m_color{ color }
	{}

	Vector2D m_pos; // actual pos to draw
	Vector2D m_move; // how much to move the element +/- need small values!
	Color m_color;
	float m_alpha = 1.0f;
};

class Background
{
public:
	Background() = default;

	constexpr Background(size_t size, float maxDist, const std::vector<Color>& colorArr) :
		m_size{ size }, m_maxDistLines{ maxDist }, m_colorArr{ colorArr }
	{}

	void draw(ImDrawList* _draw);
	void init();
private:
	void pushRandomPoints();
	void update(ParticlePoint_t& particle);
	void find(ParticlePoint_t& particle);
	void drawLine(const Vector2D& start, const Vector2D& end, const Color& color, float thickness = 1.0f);
	void drawCircleFilled(const Vector2D& pos, float radius, size_t points, const Color& color);
	void drawRectFilled(float x, float y, float width, float height, const Color& color);

	size_t m_size; // how many particless
	float m_maxDistLines; // dist between

	std::vector<ParticlePoint_t> m_particleArr;
	std::vector<Color> m_colorArr; // temp limited usage, FIXME

	ImDrawList* m_draw;
};

inline Background background;