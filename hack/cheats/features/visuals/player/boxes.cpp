#include "boxes.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>

void boxes::drawInnerOutline(const Box& box, float extraAlpha)
{
	ImRender::drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Colors::Black.getColorEditAlpha(extraAlpha));
}

void boxes::drawOuterOutline(const Box& box, float extraAlpha)
{
	ImRender::drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Colors::Black.getColorEditAlpha(extraAlpha));
}

void boxes::drawRect(const Box& box, const Color& color, float extraAlpha)
{
	ImRender::drawRect(box.x, box.y, box.w, box.h, color.getColorEditAlpha(extraAlpha));
}

void boxes::drawRectFilled(const Box& box, const Color& color, float extraAlpha)
{
	ImRender::drawRectFilled(box.x, box.y, box.w, box.h, color.getColorEditAlpha(extraAlpha));
}

void boxes::drawRectGradient(const Box& box, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float extraAlpha)
{
	ImRender::drawRectFilledMultiColor(box.x, box.y, box.w, box.h,
		color1.getColorEditAlpha(extraAlpha),
		color2.getColorEditAlpha(extraAlpha),
		color3.getColorEditAlpha(extraAlpha),
		color4.getColorEditAlpha(extraAlpha));
}

void boxes::drawBox3D(const Box& box, const Color& color, float extraAlpha)
{
	for (size_t i = 1; i < 5; i++)
	{
		// BOTTOM 0,1,2,3
		ImRender::drawLine(box.points.at(i - 1), box.points.at(i % 4), color.getColorEditAlpha(extraAlpha));
		// TOP 4,5,6,7
		ImRender::drawLine(box.points.at(i + 3), box.points.at(i % 4 + 4), color.getColorEditAlpha(extraAlpha));
		// MISSING TOP
		ImRender::drawLine(box.points.at(i - 1), box.points.at(i + 3), color.getColorEditAlpha(extraAlpha));
	}
}

void boxes::drawBoxFilled3D(const Box& box, const Color& color, float extraAlpha)
{
	const auto maybeScanned = math::grahamScan(box.points);
	if (!maybeScanned.has_value())
		return;

	auto points = maybeScanned.value();

	std::reverse(points.begin(), points.end());
	ImRender::drawPolyGon(points, color.getColorEditAlpha(extraAlpha));
}