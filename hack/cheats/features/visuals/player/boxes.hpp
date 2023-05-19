#pragma once

#include <render/BBox.hpp>
#include <config/cfgBox.hpp>

namespace boxes
{
	void drawInnerOutline(const Box& box, float extraAlpha = 1.0f);
	void drawOuterOutline(const Box& box, float extraAlpha = 1.0f);
	void drawRect(const Box& box, const Color& color, float extraAlpha = 1.0f);
	void drawRectFilled(const Box& box, const Color& color, float extraAlpha = 1.0f);
	void drawRectGradient(const Box& box, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float extraAlpha = 1.0f);
	void drawBox3D(const Box& box, const Color& color, float extraAlpha = 1.0f);
	void drawBoxFilled3D(const Box& box, const Color& color, float extraAlpha = 1.0f);
};
