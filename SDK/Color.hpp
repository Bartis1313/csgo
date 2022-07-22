#pragma once
#include <cstdint>
#include <array>
#include <stdexcept>

using ImU32 = size_t;
struct ImVec4;

// 0.0 - 1.0, you can pass ints as 0-255 though
class Color
{
public:
	Color() = default;
	constexpr Color(float r, float g, float b, float a = 1.0f)
	{
		setColor(r, g, b, a);
	}
	constexpr Color(int r, int g, int b, int a = 255)
	{
		setColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
	constexpr Color(const std::array<float, 4>& colArr)
	{
		setColor(colArr.at(0), colArr.at(1), colArr.at(2), colArr.at(3));
	}
	constexpr Color(float colArr[4])
	{
		setColor(colArr[0], colArr[1], colArr[2], colArr[3]);
	}
	constexpr Color(const Color& col, float alpha)
	{
		setColor(col.r(), col.g(), col.b(), alpha);
	}

	constexpr void setColor(float r, float g, float b, float a = 1.0f) { m_color.at(0) = r; m_color.at(1) = g; m_color.at(2) = b; m_color.at(3) = a; }
	constexpr void setAlpha(float a) { m_color.at(3) = a; }
	constexpr void setAlphaInt(int a) { m_color.at(3) = a / 255.0f; }

	_NODISCARD constexpr float r() const { return m_color.at(0); }
	_NODISCARD constexpr float g() const { return m_color.at(1); }
	_NODISCARD constexpr float b() const { return m_color.at(2); }
	_NODISCARD constexpr float a() const { return m_color.at(3); }

	_NODISCARD static ImVec4 getImguiColor(const Color& color);
	// edits alpha
	_NODISCARD Color getColorEditAlpha(const float amount) const;
	_NODISCARD Color getColorEditAlphaInt(const int amount) const;

	_NODISCARD constexpr uint8_t rMultiplied() const { return static_cast<uint8_t>(m_color.at(0) * 255.0f); }
	_NODISCARD constexpr uint8_t gMultiplied() const { return static_cast<uint8_t>(m_color.at(1) * 255.0f); }
	_NODISCARD constexpr uint8_t bMultiplied() const { return static_cast<uint8_t>(m_color.at(2) * 255.0f); }
	_NODISCARD constexpr uint8_t aMultiplied() const { return static_cast<uint8_t>(m_color.at(3) * 255.0f); }

	constexpr const float& operator[](int index) const { return m_color.at(index); }
	constexpr float& operator[](int index) { return m_color.at(index); }
	constexpr const float& at(int index) const { if (index >= m_color.size() || index < 0) throw std::runtime_error("Out of range!"); return m_color.at(index); } // as std, at() is safe
	constexpr float& at(int index) { if (index >= m_color.size() || index < 0) throw std::runtime_error("Out of range!"); return m_color.at(index); }
	constexpr bool operator == (const Color& rhs) const { return (*((uintptr_t*)this) == *((uintptr_t*)&rhs)); }
	constexpr bool operator != (const Color& rhs) const { return !(operator==(rhs)); }
	_NODISCARD static Color fromHSB(float hue, float saturation, float brightness);
	//https://gist.github.com/mjackson/5311256
	_NODISCARD static Color hslToRGB(float hue, float saturation, float lightness);
	_NODISCARD static Color rainbowColor(const float gameTime, const float multiply = 0.5f); // http://basecase.org/env/on-rainbows
private:
	std::array<float, 4> m_color;
};

// 0-255 & 1byte type
struct SDKColor
{
	SDKColor() = default;
	constexpr SDKColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: r{ r }, g{ g }, b{ b }, a{ a }
	{}
	constexpr SDKColor(const Color& color)
		: r{ color.rMultiplied() }, g{ color.gMultiplied() }, b{ color.bMultiplied() }, a{ color.aMultiplied() }
	{}

	uint8_t r, g, b, a;
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
	inline Color Blank = Color(0, 0, 0, 0);
	inline Color Orange = Color(255, 125, 0, 255);
}

_NODISCARD ImU32 U32(const Color& color);