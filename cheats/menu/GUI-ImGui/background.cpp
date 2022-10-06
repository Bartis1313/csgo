#include "background.hpp"

#include "menu.hpp"

#include <dependencies/ImGui/imgui.h>
#include <game/globals.hpp>
#include <utilities/utilities.hpp>
#include <utilities/rand.hpp>
#include <config/vars.hpp>

void Background::drawLine(const Vec2& start, const Vec2& end, const Color& color, float thickness)
{
	m_draw->AddLine(ImVec2{ start[Coord::X], start[Coord::Y] }, ImVec2{ end[Coord::X], end[Coord::Y] }, Color::U32(color), thickness);
}

void Background::drawCircleFilled(const Vec2& pos, float radius, size_t points, const Color& color)
{
	m_draw->AddCircleFilled(ImVec2{ pos[Coord::X], pos[Coord::Y] }, radius, Color::U32(color), points);
}

void Background::drawRectFilled(float x, float y, float width, float height, const Color& color)
{
	m_draw->AddRectFilled(ImVec2{ x, y }, ImVec2{ x + width, y + height }, Color::U32(color));
}

void Background::init()
{
	m_size = static_cast<size_t>(vars::styling->size);
	m_maxDistLines = vars::styling->distance;
	m_colorArr =
	{
		vars::styling->color1(),
		vars::styling->color2(),
		vars::styling->color3()
	};

	pushRandomPoints();
}

void Background::pushRandomPoints()
{
	m_particleArr.clear();

	float speed = vars::styling->speed;
	for (size_t i = 0; i < m_size; i++)
	{
		m_particleArr.emplace_back(
			ParticlePoint_t
			{
				Vec2{ static_cast<float>(Random::getRandom<size_t>(0, globals::screenX)), static_cast<float>(Random::getRandom<size_t>(0, globals::screenY)) }, // pos
				Vec2{ Random::getRandom<float>(-0.1f, 0.1f) * speed, Random::getRandom<float>(-0.1f, 0.1f) * speed }, // move
				Color	{ m_colorArr.at(Random::getRandom<size_t>(0, m_colorArr.size() - 1)) } // color
			});
	}
}

void Background::update(ParticlePoint_t& particle)
{
	if (particle.m_pos[Coord::X] > globals::screenX || particle.m_pos[Coord::X] < 0)
		particle.m_move[Coord::X] = -particle.m_move[Coord::X];

	if (particle.m_pos[Coord::Y] > globals::screenY || particle.m_pos[Coord::Y] < 0)
		particle.m_move[Coord::Y] = -particle.m_move[Coord::Y];

	particle.m_pos += particle.m_move;
}

void Background::find(ParticlePoint_t& particle)
{
	for (auto& el : m_particleArr)
	{
		if (auto dis = particle.m_pos.distTo(el.m_pos); dis < m_maxDistLines)
		{
			el.m_alpha = (m_maxDistLines - dis) / m_maxDistLines;
			drawLine(particle.m_pos, el.m_pos, particle.m_color.getColorEditAlpha(el.m_alpha));
		}
	}
}

void Background::draw(ImDrawList* _draw)
{
	if (globals::isShutdown)
		return;

	m_draw = _draw;

	static bool bOnce = [this]() // init
	{		
		init();

		return true;
	} ();

	if (!menu.isMenuActive())
		return;

	if (!vars::styling->background)
		return;

	drawRectFilled(0.0f, 0.0f, static_cast<float>(globals::screenX), static_cast<float>(globals::screenY), Colors::Grey);

	for (auto& el : m_particleArr)
	{
		update(el);
		find(el);
		drawCircleFilled(el.m_pos, 2, 6, Colors::Black);
	}
}