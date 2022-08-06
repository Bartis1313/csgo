#include "boxes.hpp"

#include "../../../../../SDK/CGlobalVars.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"
#include "../../../../../utilities/math/math.hpp"


void BoxesDraw::drawBox2D(const Box& box)
{
	CfgColor cfgCol = config.get<CfgColor>(vars.cBox);
	Color outlineCol = Colors::Black.getColorEditAlpha(cfgCol.getColor().a());
	bool outlined = config.get<bool>(vars.bBoxOutlined);

	if (outlined)
	{
		imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, outlineCol);
		imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, outlineCol);
	}
	imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol.getColor());
}


void BoxesDraw::drawBox2DFilled(const Box& box)
{
	CfgColor fill = config.get<CfgColor>(vars.cBoxFill);

	if (!config.get<bool>(vars.bBoxMultiColor))
		imRender.drawRectFilled(box.x, box.y, box.w, box.h, fill.getColor());
	else
	{
		float speed = config.get<float>(vars.fBoxMultiColor);
		float time = interfaces::globalVars->m_curtime;

		imRender.drawRectFilledMultiColor(box.x, box.y, box.w, box.h,
			Color::rainbowColor(time, speed).getColorEditAlpha(fill.getColor().a()),
			Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(fill.getColor().a()),
			Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(fill.getColor().a()),
			Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(fill.getColor().a()));
	}
	drawBox2D(box);
}

static ImVec2 operator-(const ImVec2& v, float val)
{
	return ImVec2{ v.x - val, v.y - val };
}

static ImVec2 operator+(const ImVec2& v, float val)
{
	return ImVec2{ v.x + val, v.y + val };
}

void BoxesDraw::drawBox3DFilled(const Box3D& box, const float thickness)
{
	CfgColor color = config.get<CfgColor>(vars.cBox);
	CfgColor fill = config.get<CfgColor>(vars.cBoxFill);
	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false; // looks bad on 3d

	auto points = math::grahamScan(box.points);
	std::reverse(points.begin(), points.end());
	imRender.drawPolyGon(points, fill.getColor());

	Color outlineCol = Colors::Black.getColorEditAlpha(color.getColor().a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

void BoxesDraw::drawBox3DFilledMultiColor(const Box3D& box, const float thickness)
{
	CfgColor color = config.get<CfgColor>(vars.cBox);
	CfgColor fill = config.get<CfgColor>(vars.cBoxFill);
	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	float speed = config.get<float>(vars.fBoxMultiColor);
	float time = interfaces::globalVars->m_curtime;

	std::vector colors =
	{
		U32(Color::rainbowColor(time, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 4.0f, speed).getColorEditAlpha(fill.getColor().a())),
		U32(Color::rainbowColor(time + 5.0f, speed).getColorEditAlpha(fill.getColor().a())),
	};

	auto points = math::grahamScan(box.points);
	size_t delta = colors.size() - points.size();
	std::reverse(points.begin(), points.end());
	imRender.drawPolyGonMultiColor(points, colors); // any way to make it smooth trnasmition in vertex? I couldn't think of

	Color outlineCol = Colors::Black.getColorEditAlpha(color.getColor().a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}

void BoxesDraw::drawBox3D(const Box3D& box, const float thickness)
{
	CfgColor color = config.get<CfgColor>(vars.cBox);
	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	Color outlineCol = Colors::Black.getColorEditAlpha(color.getColor().a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColor(), thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}
