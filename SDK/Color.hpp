#pragma once
#include <cstdint>

class Color
{
public:
	Color()
	{
		setColor(255, 255, 255, 255);
	}

	Color(int r, int g, int b, int a = 255)
	{
		setColor(r, g, b, a);
	}

	void setColor(int r, int g, int b, int a = 255)
	{
		color[0] = static_cast<uint8_t>(r);
		color[1] = static_cast<uint8_t>(g);
		color[2] = static_cast<uint8_t>(b);
		color[3] = static_cast<uint8_t>(a);
	}

	void getColor(int& r, int& g, int& b, int& a) const
	{
		r = color[0];
		g = color[1];
		b = color[2];
		a = color[3];
	}

	void setRawColor(int color32)
	{
		*((int*)this) = color32;
	}

	int getRawColor() const
	{
		return *((int*)this);
	}

	inline int r() const { return color[0]; }
	inline int g() const { return color[1]; }
	inline int b() const { return color[2]; }
	inline int a() const { return color[3]; }

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

	Color& operator=(const Color& rhs)
	{
		setRawColor(rhs.getRawColor());
		return *this;
	}

	static Color fromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - static_cast<uint8_t>(h);
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return Color(
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(t * 255),
				static_cast<uint8_t>(p * 255)
			);
		}
		else if (h < 2)
		{
			return Color(
				static_cast<uint8_t>(q * 255),
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(p * 255)
			);
		}
		else if (h < 3)
		{
			return Color(
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(t * 255)
			);
		}
		else if (h < 4)
		{
			return Color(
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(q * 255),
				static_cast<uint8_t>(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return Color(
				static_cast<uint8_t>(t * 255),
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(brightness * 255)
			);
		}
		else
		{
			return Color(
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(q * 255)
			);
		}
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
}