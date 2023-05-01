#include "background.hpp"

#include "../menu.hpp"

#include <imgui.h>
#include <cheats/game/globals.hpp>
#include <utilities/utilities.hpp>
#include <utilities/rand.hpp>
#include <config/vars.hpp>

#include <cheats/hooks/present.hpp>

namespace
{
	struct BackgroundHandle : hooks::Present
	{
		BackgroundHandle()
		{
			this->registerRun(background::draw);
		}
	} backgroundHandle;
}

namespace background
{
	void pushRandomPoints();
	void update(ParticlePoint_t& particle);
	void find(ParticlePoint_t& particle);

	void drawLine(const Vec2& start, const Vec2& end, const Color& color, float thickness = 1.0f);
	void drawCircleFilled(const Vec2& pos, float radius, size_t points, const Color& color);
	void drawRectFilled(float x, float y, float width, float height, const Color& color);

	size_t particlesCount; // how many particless
	float maxDistance; // dist between

	std::vector<ParticlePoint_t> particles;
	std::vector<Color> colors;

	ImDrawList* ImDraw;
}

void background::drawLine(const Vec2& start, const Vec2& end, const Color& color, float thickness)
{
	ImDraw->AddLine(ImVec2{ start[Coord::X], start[Coord::Y] }, ImVec2{ end[Coord::X], end[Coord::Y] }, Color::U32(color), thickness);
}

void background::drawCircleFilled(const Vec2& pos, float radius, size_t points, const Color& color)
{
	ImDraw->AddCircleFilled(ImVec2{ pos[Coord::X], pos[Coord::Y] }, radius, Color::U32(color), points);
}

void background::drawRectFilled(float x, float y, float width, float height, const Color& color)
{
	ImDraw->AddRectFilled(ImVec2{ x, y }, ImVec2{ x + width, y + height }, Color::U32(color));
}

void background::init()
{
	particlesCount = static_cast<size_t>(vars::styling->size);
	maxDistance = vars::styling->distance;
	colors =
	{
		vars::styling->color1(),
		vars::styling->color2(),
		vars::styling->color3()
	};

	pushRandomPoints();
}

void background::pushRandomPoints()
{
	particles.clear();

	float speed = vars::styling->speed;
	for (size_t i = 0; i < particlesCount; i++)
	{
		particles.emplace_back(
			ParticlePoint_t
			{
				Vec2{ static_cast<float>(Random::getRandom<size_t>(0, globals::screenX)), static_cast<float>(Random::getRandom<size_t>(0, globals::screenY)) }, // pos
				Vec2{ Random::getRandom<float>(-0.1f, 0.1f) * speed, Random::getRandom<float>(-0.1f, 0.1f) * speed }, // move
				Color	{ colors.at(Random::getRandom<size_t>(0, colors.size() - 1)) } // color
			});
	}
}

void background::update(ParticlePoint_t& particle)
{
	// not working perfectly
	/*const auto circlePos = Vec2{ float(globals::mouseX), float(globals::mouseY) };
	float distToCircle = particle.m_pos.distTo(circlePos);
	float circleRadius = 60.0f;
	drawCircleFilled(circlePos, circleRadius, 64, Colors::Green.getColorEditAlpha(0.1f));

	if (distToCircle < circleRadius)
	{
		auto alp = (circleRadius - distToCircle) / circleRadius;

		Vec2 dir = (particle.m_pos - circlePos) * alp;
		particle.m_pos += dir * (circleRadius - distToCircle) * 1.2f;

		particle.m_move = particle.m_move * -1.0f;
	}*/

	if (particle.m_pos[Coord::X] > globals::screenX || particle.m_pos[Coord::X] < 0)
		particle.m_move[Coord::X] = -particle.m_move[Coord::X];

	if (particle.m_pos[Coord::Y] > globals::screenY || particle.m_pos[Coord::Y] < 0)
		particle.m_move[Coord::Y] = -particle.m_move[Coord::Y];

	particle.m_pos += particle.m_move;
}

void background::find(ParticlePoint_t& particle)
{
	for (auto& el : particles)
	{
		if (const auto dis = particle.m_pos.distTo(el.m_pos); dis < maxDistance)
		{
			el.m_alpha = (maxDistance - dis) / maxDistance;
			drawLine(particle.m_pos, el.m_pos, particle.m_color.getColorEditAlpha(el.m_alpha));
		}
	}
}

void background::draw()
{
	if (globals::isShutdown)
		return;

	ImDraw = ImGui::GetBackgroundDrawList();

	if (!ImGuiMenu::active)
		return;

	if (!vars::styling->background)
		return;

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { init(); });

	drawRectFilled(0.0f, 0.0f, static_cast<float>(globals::screenX), static_cast<float>(globals::screenY), Colors::Grey);

	for (auto& el : particles)
	{
		update(el);
		find(el);
		drawCircleFilled(el.m_pos, 2, 6, Colors::Black);
	}
}