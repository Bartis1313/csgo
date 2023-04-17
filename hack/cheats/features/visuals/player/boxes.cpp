#include "boxes.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>

void BoxesDraw::drawBox2D(const Box& box, bool isDormant, float dormacyAlpha)
{
	Color color = isDormant
		? vars::visuals->esp->boxes->color().getColorEditAlpha(dormacyAlpha)
		: vars::visuals->esp->boxes->color();

	Color outlineCol = isDormant
		? Colors::Black.getColorEditAlpha(dormacyAlpha)
		: Colors::Black.getColorEditAlpha(color.a());

	bool outlined = vars::visuals->esp->boxes->outline;

	if (outlined)
	{
		ImRender::drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, outlineCol);
		ImRender::drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, outlineCol);
	}
	ImRender::drawRect(box.x, box.y, box.w, box.h, color);
}


void BoxesDraw::drawBox2DFilled(const Box& box, bool isDormant, float dormacyAlpha)
{
	Color fill = isDormant
		? vars::visuals->esp->boxes->fill().getColorEditAlpha(dormacyAlpha)
		: vars::visuals->esp->boxes->fill();

	if (!vars::visuals->esp->boxes->multiColor)
		ImRender::drawRectFilled(box.x, box.y, box.w, box.h, fill);
	else
	{
		float speed = vars::visuals->esp->boxes->multiColorSpeed;
		float time = memory::interfaces::globalVars->m_curtime;

		float alpha = fill.a();

		ImRender::drawRectFilledMultiColor(box.x, box.y, box.w, box.h,
			Color::rainbowColor(time, speed).getColorEditAlpha(alpha),
			Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(alpha),
			Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(alpha),
			Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(alpha));
	}
	drawBox2D(box, isDormant, dormacyAlpha);
}

static ImVec2 operator-(const ImVec2& v, float val)
{
	return ImVec2{ v.x - val, v.y - val };
}

static ImVec2 operator+(const ImVec2& v, float val)
{
	return ImVec2{ v.x + val, v.y + val };
}

void BoxesDraw::drawBox3DFilled(const Box& box, bool isDormant, float dormacyAlpha, float thickness)
{
	Color fill = isDormant
		? vars::visuals->esp->boxes->fill().getColorEditAlpha(dormacyAlpha)
		: vars::visuals->esp->boxes->fill();

	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false; // looks bad on 3d

	auto maybeScanned = math::grahamScan(box.points);
	if (!maybeScanned.has_value())
		return;

	auto points = maybeScanned.value();

	std::reverse(points.begin(), points.end());
	ImRender::drawPolyGon(points, fill);

	drawBox3D(box, isDormant, dormacyAlpha, thickness);
}

void BoxesDraw::drawBox3DFilledMultiColor(const Box& box, bool isDormant, float dormacyAlpha, float thickness)
{
	Color fill = isDormant
		? vars::visuals->esp->boxes->fill().getColorEditAlpha(dormacyAlpha)
		: vars::visuals->esp->boxes->fill();

	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	float speed = vars::visuals->esp->boxes->multiColorSpeed;
	float time = memory::interfaces::globalVars->m_curtime;

	float alpha = fill.a();

	std::vector colors =
	{
		Color::U32(Color::rainbowColor(time, speed).getColorEditAlpha(alpha)),
		Color::U32(Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(alpha)),
		Color::U32(Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(alpha)),
		Color::U32(Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(alpha)),
		Color::U32(Color::rainbowColor(time + 4.0f, speed).getColorEditAlpha(alpha)),
		Color::U32(Color::rainbowColor(time + 5.0f, speed).getColorEditAlpha(alpha)),
	};

	auto maybeScanned = math::grahamScan(box.points);
	if (!maybeScanned.has_value())
		return;

	auto points = maybeScanned.value();

	size_t delta = colors.size() - points.size();
	std::reverse(points.begin(), points.end());
	ImRender::drawPolyGonMultiColor(points, colors); // any way to make it smooth trnasmition in vertex? I couldn't think of

	drawBox3D(box, isDormant, dormacyAlpha, thickness);
}

void BoxesDraw::drawBox3D(const Box& box, bool isDormant, float dormacyAlpha, float thickness)
{
	Color color = isDormant
		? vars::visuals->esp->boxes->color().getColorEditAlpha(dormacyAlpha)
		: vars::visuals->esp->boxes->color();

	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	Color outlineCol = isDormant
		? Colors::Black.getColorEditAlpha(dormacyAlpha)
		: Colors::Black.getColorEditAlpha(color.a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		ImRender::drawLine(box.points.at(i - 1), box.points.at(i % 4), color, thickness);
		if (outlined)
			ImRender::drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		ImRender::drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color, thickness);
		if (outlined)
			ImRender::drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		ImRender::drawLine(box.points.at(i - 1), box.points.at(i + 3), color, thickness);
		if (outlined)
			ImRender::drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}
