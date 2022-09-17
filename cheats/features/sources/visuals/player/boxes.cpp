#include "boxes.hpp"

#include "../../../../../SDK/CGlobalVars.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"
#include "../../../../../utilities/math/math.hpp"

void BoxesDraw::drawBox2D(const Box& box, bool isDormant, float dormacyAlpha)
{
	Color cfgCol = isDormant
		? config.get<CfgColor>(vars.cBox).getColor().getColorEditAlpha(dormacyAlpha)
		: config.get<CfgColor>(vars.cBox).getColor();

	Color outlineCol = isDormant
		? Colors::Black.getColorEditAlpha(dormacyAlpha)
		: Colors::Black.getColorEditAlpha(cfgCol.a());

	bool outlined = config.get<bool>(vars.bBoxOutlined);

	if (outlined)
	{
		imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, outlineCol);
		imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, outlineCol);
	}
	imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol.getColorEditAlpha(dormacyAlpha));
}


void BoxesDraw::drawBox2DFilled(const Box& box, bool isDormant, float dormacyAlpha)
{
	Color fill = config.get<CfgColor>(vars.cBoxFill).getColor();
	if (isDormant)
		fill = fill.getColorEditAlpha(dormacyAlpha);

	if (!config.get<bool>(vars.bBoxMultiColor))
		imRender.drawRectFilled(box.x, box.y, box.w, box.h, fill);
	else
	{
		float speed = config.get<float>(vars.fBoxMultiColor);
		float time = interfaces::globalVars->m_curtime;

		float alpha = fill.a();

		imRender.drawRectFilledMultiColor(box.x, box.y, box.w, box.h,
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
	Color fill = config.get<CfgColor>(vars.cBoxFill).getColor();
	if (isDormant)
		fill = fill.getColorEditAlpha(dormacyAlpha);

	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false; // looks bad on 3d

	auto points = math::grahamScan(box.points);
	std::reverse(points.begin(), points.end());
	imRender.drawPolyGon(points, fill);

	drawBox3D(box, isDormant, dormacyAlpha, thickness);
}

void BoxesDraw::drawBox3DFilledMultiColor(const Box& box, bool isDormant, float dormacyAlpha, float thickness)
{
	Color fill = config.get<CfgColor>(vars.cBoxFill).getColor();
	if (isDormant)
		fill = fill.getColorEditAlpha(dormacyAlpha);

	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	float speed = config.get<float>(vars.fBoxMultiColor);
	float time = interfaces::globalVars->m_curtime;

	float alpha = fill.a();

	std::vector colors =
	{
		U32(Color::rainbowColor(time, speed).getColorEditAlpha(alpha)),
		U32(Color::rainbowColor(time + 1.0f, speed).getColorEditAlpha(alpha)),
		U32(Color::rainbowColor(time + 2.0f, speed).getColorEditAlpha(alpha)),
		U32(Color::rainbowColor(time + 3.0f, speed).getColorEditAlpha(alpha)),
		U32(Color::rainbowColor(time + 4.0f, speed).getColorEditAlpha(alpha)),
		U32(Color::rainbowColor(time + 5.0f, speed).getColorEditAlpha(alpha)),
	};

	auto points = math::grahamScan(box.points);
	size_t delta = colors.size() - points.size();
	std::reverse(points.begin(), points.end());
	imRender.drawPolyGonMultiColor(points, colors); // any way to make it smooth trnasmition in vertex? I couldn't think of

	drawBox3D(box, isDormant, dormacyAlpha, thickness);
}

void BoxesDraw::drawBox3D(const Box& box, bool isDormant, float dormacyAlpha, float thickness)
{
	Color color = isDormant
		? config.get<CfgColor>(vars.cBox).getColor().getColorEditAlpha(dormacyAlpha)
		: config.get<CfgColor>(vars.cBox).getColor();

	bool outlined = /*config.get<bool>(vars.bBoxOutlined)*/ false;

	Color outlineCol = isDormant
		? Colors::Black.getColorEditAlpha(dormacyAlpha)
		: Colors::Black.getColorEditAlpha(color.a());

	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		imRender.drawLine(box.points.at(i - 1), box.points.at(i % 4), color, thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i % 4) + 1.0f - thickness, outlineCol);
		// TOP 4,5,6,7
		imRender.drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color, thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i + 3) + 1.0f - thickness, box.points.at(i % 4 + 4) + 1.0f - thickness, outlineCol);
		// MISSING TOP
		imRender.drawLine(box.points.at(i - 1), box.points.at(i + 3), color, thickness);
		if (outlined)
			imRender.drawLine(box.points.at(i - 1) + 1.0f - thickness, box.points.at(i + 3) + 1.0f - thickness, outlineCol);
	}
}
