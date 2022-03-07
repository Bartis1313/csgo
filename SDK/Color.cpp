#include "Color.hpp"
#include "../dependencies/ImGui/imgui.h" // for struct

float Rainbow::hue = 0.0f;

constexpr void Color::setColor(float r, float g, float b, float a)
{
	m_color.at(0) = r;
	m_color.at(1) = g;
	m_color.at(2) = b;
	m_color.at(3) = a;
}

constexpr void Color::setAlpha(float a)
{
	m_color.at(3) = a;
}

ImVec4 Color::getImguiColor(const Color& color)
{
	return ImVec4{ color.r(), color.g(), color.b(), color.a() };
}

constexpr Color& Color::getColorEditAlpha(const float amount)
{
	m_color.at(3) = amount;
	return *this;
}

Color Color::fromHSB(float hue, float saturation, float brightness)
{
	float h = hue == 1.0f ? 0.0f : hue * 6.0f;
	float f = h - h;
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	if (h < 1)
	{
		return Color(brightness, t, p);
	}
	else if (h < 2)
	{
		return Color(q, brightness, p);
	}
	else if (h < 3)
	{
		return Color(p, brightness, t);
	}
	else if (h < 4)
	{
		return Color(p, q, brightness);
	}
	else if (h < 5)
	{
		return Color(t, p, brightness);
	}
	else
	{
		return Color(brightness, p, q);
	}
}

Color Color::hslToRGB(float hue, float saturation, float lightness)
{
	static auto huetoRGB = [](float p, float q, float t)
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

	return Color(r, g, b);
}

Color Rainbow::drawRainbow(float frameTime, float saturation, float alpha, float multiply)
{
	hue += frameTime * multiply;
	if (hue > 1.0f)
		hue = 0.0f;
	return Color::fromHSB(hue, saturation, alpha);
}

ImU32 U32(const Color& color)
{
	return ImGui::GetColorU32(Color::getImguiColor(color));
}