#pragma once
#include <cstdint>
#include <array>
#include <algorithm>

#undef min
#undef max

class Color
{
public:
	Color();
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	Color(const Color& clr, uint8_t a);

	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void setAlpha(uint8_t a);

	_NODISCARD uint8_t r() const { return m_color.at(0); }
	_NODISCARD uint8_t g() const { return m_color.at(1); }
	_NODISCARD uint8_t b() const { return m_color.at(2); }
	_NODISCARD uint8_t a() const { return m_color.at(3); }

	_NODISCARD float rDevided() const { return m_color.at(0) / 255.0f; }
	_NODISCARD float gDevided() const { return m_color.at(1) / 255.0f; }
	_NODISCARD float bDevided() const { return m_color.at(2) / 255.0f; }
	_NODISCARD float aDevided() const { return m_color.at(3) / 255.0f; }

	const uint8_t& operator[](int index) const { return m_color.at(index); }
	const uint8_t& at(int index) const { return m_color.at(index); }
	bool operator == (const Color& rhs) const {	return (*((uintptr_t*)this) == *((uintptr_t*)&rhs)); }
	bool operator != (const Color& rhs) const { return !(operator==(rhs)); }
	static Color fromHSB(float hue, float saturation, float brightness);
	//https://gist.github.com/mjackson/5311256
	static Color hslToRGB(float hue, float saturation, float lightness);
	static float getHueFromColor(const Color& clr);
private:
	std::array<uint8_t, 4> m_color;
};

class Rainbow
{
public:
	Color drawRainbow(float frameTime, float saturation, float alpha, float multiply = 0.05f);
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
	inline Color Coral = Color(255, 127, 80, 255);
	inline Color Cyan = Color(0, 255, 255, 255);
}