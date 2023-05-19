#include "Color.hpp"

#include <imgui_impl_dx9.h>

Color Color::getColorEditAlpha(const float amount) const
{
	Color clr = *this;
	clr.at(3) = amount;
	return clr;
}

Color Color::getColorEditAlphaInt(const int amount) const
{
	Color clr = *this;
	clr.at(3) = amount / 255.0f;
	return clr;
}

ImU32 Color::U32(const Color& color)
{
	return ImColor{ color.r(), color.g(), color.b(), color.a() };
}

#include <cmath>
#include <numbers>

Color Color::rainbowColor(const float gameTime, const float multiply)
{
	return Color
	{
		std::cos(gameTime * multiply) * 0.5f + 0.5f,
		std::cos(gameTime * multiply - 2.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f,
		std::cos(gameTime * multiply - 4.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f 
	};
}

Color Color::healthBased(uint8_t health, uint8_t alpha)
{
	const int g = static_cast<int>(health * 2.55f);
	const int r = 255 - g;
	return Color{ r, g, 0, alpha };
}
