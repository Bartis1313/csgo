#include "Color.hpp"

float Rainbow::hue = 0.0f;

Color::Color()
{
	setColor(255, 255, 255, 255);
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	setColor(r, g, b, a);
}

Color::Color(const Color& clr, uint8_t a)
{
	setColor(clr.r(), clr.g(), clr.b(), a);
}

void Color::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	m_color.at(0) = r;
	m_color.at(1) = g;
	m_color.at(2) = b;
	m_color.at(3) = a;
}

void Color::setAlpha(uint8_t a)
{
	m_color.at(3) = a;
}

Color Color::fromHSB(float hue, float saturation, float brightness)
{
	float h = hue == 1.0f ? 0.0f : hue * 6.0f;
	float f = h - static_cast<uint8_t>(h);
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	if (h < 1)
	{
		return Color(
			static_cast<uint8_t>(brightness * 255.0f),
			static_cast<uint8_t>(t * 255.0f),
			static_cast<uint8_t>(p * 255.0f)
		);
	}
	else if (h < 2)
	{
		return Color(
			static_cast<uint8_t>(q * 255.0f),
			static_cast<uint8_t>(brightness * 255.0f),
			static_cast<uint8_t>(p * 255.0f)
		);
	}
	else if (h < 3)
	{
		return Color(
			static_cast<uint8_t>(p * 255.0f),
			static_cast<uint8_t>(brightness * 255.0f),
			static_cast<uint8_t>(t * 255.0f)
		);
	}
	else if (h < 4)
	{
		return Color(
			static_cast<uint8_t>(p * 255.0f),
			static_cast<uint8_t>(q * 255.0f),
			static_cast<uint8_t>(brightness * 255.0f)
		);
	}
	else if (h < 5)
	{
		return Color(
			static_cast<uint8_t>(t * 255.0f),
			static_cast<uint8_t>(p * 255.0f),
			static_cast<uint8_t>(brightness * 255.0f)
		);
	}
	else
	{
		return Color(
			static_cast<uint8_t>(brightness * 255.0f),
			static_cast<uint8_t>(p * 255.0f),
			static_cast<uint8_t>(q * 255.0f)
		);
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

	return Color(
		static_cast<uint8_t>(r * 255),
		static_cast<uint8_t>(g * 255),
		static_cast<uint8_t>(b * 255));
}

float Color::getHueFromColor(const Color& clr)
{
	float r = (std::clamp<int>(clr[0], 0, 255) / 255.0f);
	float g = (std::clamp<int>(clr[1], 0, 255) / 255.0f);
	float b = (std::clamp<int>(clr[2], 0, 255) / 255.0f);
	float max = std::max(std::max(r, g), b);
	float min = std::min(std::min(r, g), b);
	float delta = max - min;

	if (!delta)
		return 0.0f;

	float hue = 0.0f;

	if (max == r)
	{
		hue = (g - b) / delta;
	}
	else if (max == g)
	{
		hue = 2.0f + (b - r) / delta;
	}
	else
	{
		hue = 4.0f + (r - g) / delta;
	}

	hue *= 60.0f;

	if (hue < 0.0f)
		hue += 360.0f;

	return hue / 360.f;
}

Color Rainbow::drawRainbow(float frameTime, float saturation, float alpha, float multiply)
{
	hue += frameTime * multiply;
	if (hue > 1.0f)
		hue = 0.0f;
	return Color::fromHSB(hue, saturation, alpha);
}