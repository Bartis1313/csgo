#pragma once
#include <cstdint>
#include <array>

#undef min
#undef max

class Color
{
public:
	Color()
	{
		setColor(255, 255, 255, 255);
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
	{
		setColor(r, g, b, a);
	}

	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}

	void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
	{
		r = color[0];
		g = color[1];
		b = color[2];
		a = color[3];
	}

	Color getColorWithAlpha(const Color& color, uint8_t a) const
	{
		return Color(color.r(), color.g(), color.b(), a);
	}

	// useful for future, and guis
	std::array<float, 4> getModulate() const
	{
		return { rDevided(), gDevided() , bDevided() , aDevided() };
	}

	inline uint8_t r() const { return color[0]; }
	inline uint8_t g() const { return color[1]; }
	inline uint8_t b() const { return color[2]; }
	inline uint8_t a() const { return color[3]; }

	inline float rDevided() const { return color[0] / 255.0f; }
	inline float gDevided() const { return color[1] / 255.0f; }
	inline float bDevided() const { return color[2] / 255.0f; }
	inline float aDevided() const { return color[3] / 255.0f; }

	uint8_t& operator[](int index)
	{
		return color[index];
	}

	const uint8_t& operator[](int index) const
	{
		return color[index];
	}

	bool operator == (const Color& rhs) const
	{
		return (*((int*)this) == *((int*)&rhs));
	}

	bool operator != (const Color& rhs) const
	{
		return !(operator==(rhs));
	}

	static Color fromHSB(float hue, float saturation, float brightness)
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
	//https://gist.github.com/mjackson/5311256
	static Color hslToRGB(float hue, float saturation, float lightness)
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

	static float getHueFromColor(const Color& clr)
	{
		float r = (std::clamp<int>(clr[0], 0, 255) / 255.0f);
		float g = (std::clamp<int>(clr[1], 0, 255) / 255.0f);
		float b = (std::clamp<int>(clr[2], 0, 255) / 255.0f);
		float max = std::max(std::max(r, g), b);
		float min = std::max(std::max(r, g), b);
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
private:
	uint8_t color[4];
};

class Rainbow
{
public:
	Color drawRainbow(float frameTime, float saturation, float alpha, float multiply = 0.05f)
	{
		hue += frameTime * multiply;
		if (hue > 1.0f)
			hue = 0.0f;
		return Color::fromHSB(hue, saturation, alpha);
	}
private:
	static float hue;
};

namespace Colors
{
	inline Color White = Color(255, 255, 255, 255);
	inline Color Black = Color(0, 0, 0, 255);
	inline Color Red = Color(255, 0, 0, 255);
	inline Color Green = Color(0, 255, 0, 255);
	inline Color DarkBlue = Color(0, 0, 255, 255);
	inline Color LightBlue = Color(0, 140, 250, 255);
	inline Color Grey = Color(128, 128, 128, 128);
	inline Color Yellow = Color(255, 255, 0, 255);
	inline Color Purple = Color(140, 20, 252, 255);
	inline Color Turquoise = Color(60, 210, 200, 255);
	inline Color Palevioletred = Color(220, 110, 150, 255);
	inline Color Pink = Color(255, 100, 180, 255);
}