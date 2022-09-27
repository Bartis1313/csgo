#include "background.hpp"

#include "menu.hpp"

#include <dependencies/ImGui/imgui.h>
#include <game/globals.hpp>
#include <utilities/utilities.hpp>
#include <utilities/rand.hpp>
#include <config/vars.hpp>

void Background::drawLine(const Vector2D& start, const Vector2D& end, const Color& color, float thickness)
{
	m_draw->AddLine(ImVec2{ start.x, start.y }, ImVec2{ end.x, end.y }, Color::U32(color), thickness);
}

void Background::drawCircleFilled(const Vector2D& pos, float radius, size_t points, const Color& color)
{
	m_draw->AddCircleFilled(ImVec2{ pos.x, pos.y }, radius, Color::U32(color), points);
}

void Background::drawRectFilled(float x, float y, float width, float height, const Color& color)
{
	m_draw->AddRectFilled(ImVec2{ x, y }, ImVec2{ x + width, y + height }, Color::U32(color));
}

void Background::init()
{
	m_size = static_cast<size_t>(config.get<int>(vars.iBackgroundSize));
	m_maxDistLines = config.get<float>(vars.fBackgroundDistance);
	m_colorArr =
	{
		config.get<CfgColor>(vars.cBackGround1).getColor(),
		config.get<CfgColor>(vars.cBackGround2).getColor(),
		config.get<CfgColor>(vars.cBackGround3).getColor()
	};

	pushRandomPoints();
}

void Background::pushRandomPoints()
{
	m_particleArr.clear();

	float speed = config.get<float>(vars.fBackground);
	for (size_t i = 0; i < m_size; i++)
	{
		m_particleArr.emplace_back(
			ParticlePoint_t
			{
				Vector2D{ static_cast<float>(Random::getRandom<size_t>(0, globals::screenX)), static_cast<float>(Random::getRandom<size_t>(0, globals::screenY)) }, // pos
				Vector2D{ Random::getRandom<float>(-0.1f, 0.1f) * speed, Random::getRandom<float>(-0.1f, 0.1f) * speed }, // move
				Color	{ m_colorArr.at(Random::getRandom<size_t>(0, m_colorArr.size() - 1)) } // color
			});
	}
}

void Background::update(ParticlePoint_t& particle)
{
	if (particle.m_pos.x > globals::screenX || particle.m_pos.x < 0)
		particle.m_move.x = -particle.m_move.x;

	if (particle.m_pos.y > globals::screenY || particle.m_pos.y < 0)
		particle.m_move.y = -particle.m_move.y;

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

	if (!config.get<bool>(vars.bBackround))
		return;

	drawRectFilled(0.0f, 0.0f, static_cast<float>(globals::screenX), static_cast<float>(globals::screenY), Colors::Grey);

	for (auto& el : m_particleArr)
	{
		update(el);
		find(el);
		drawCircleFilled(el.m_pos, 2, 6, Colors::Black);
	}
}