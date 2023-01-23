#include "Color.hpp"

#include <imgui_impl_dx9.h>

ImVec4 Color::getImguiColor(const Color& color)
{
	return ImVec4{ color.r(), color.g(), color.b(), color.a() };
}

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

Color Color::fromHSB(float hue, float saturation, float brightness)
{
	float h = hue == 1.0f ? 0.0f : hue * 6.0f;
	float f = h - h;
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	if (h < 1.0f)
	{
		return Color{ brightness, t, p };
	}
	else if (h < 2.0f)
	{
		return Color{ q, brightness, p };
	}
	else if (h < 3.0f)
	{
		return Color{ p, brightness, t };
	}
	else if (h < 4.0f)
	{
		return Color{ p, q, brightness };
	}
	else if (h < 5.0f)
	{
		return Color{ t, p, brightness };
	}
	else
	{
		return Color{ brightness, p, q };
	}
}

Color Color::hslToRGB(float hue, float saturation, float lightness)
{
	auto huetoRGB = [](float p, float q, float t)
	{
		if (t < 0.0f)
			t += 1.0f;
		if (t > 1.0f)
			t -= 1.0f;
		if (t < 1.0f / 6.0f)
			return p + (q - p) * 6.0f * t;
		if (t < 1.0f / 2.0f)
			return q;
		if (t < 2.0f / 3.0f)
			return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
		return p;
	};

	float r, g, b;

	if (saturation == 0.0f)
		r = g = b = lightness;
	else
	{
		float q = lightness < 0.5f ? lightness * (1.0f + saturation) : lightness + saturation - lightness * saturation;
		float p = 2.0f * lightness - q;
		r = huetoRGB(p, q, hue + 1.0f / 3.0f);
		g = huetoRGB(p, q, hue);
		b = huetoRGB(p, q, hue - 1.0f / 3.0f);
	}

	return Color{ r, g, b };
}

ImU32 Color::U32(const Color& color)
{
	return ImGui::GetColorU32(Color::getImguiColor(color));
}

#include <cmath>
#include <numbers>

Color Color::rainbowColor(const float gameTime, const float multiply)
{
	return Color(
		std::cos(gameTime * multiply) * 0.5f + 0.5f,
		std::cos(gameTime * multiply - 2.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f,
		std::cos(gameTime * multiply - 4.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f);
}

Color Color::healthBased(uint8_t health, uint8_t alpha)
{
	int g = static_cast<int>(health * 2.55f);
	int r = 255 - g;
	return Color(r, g, 0, alpha);
}
