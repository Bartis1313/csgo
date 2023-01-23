#pragma once

#include <render/Color.hpp>

class CfgColor
{
public:
	CfgColor() = default;
	constexpr CfgColor(const Color& color, bool rainbow = false, float speed = 0.5f)
		: m_color{ color }, m_rainbow{ rainbow }, m_speed{ speed }
	{}

	constexpr CfgColor& operator=(const CfgColor& other)
	{
		this->m_color = other.getColor();
		this->m_rainbow = other.getRainbow();
		this->m_speed = other.getSpeed();
		//this->refresh();
		return *this;
	}
	constexpr CfgColor& operator=(const Color& other)
	{
		this->m_color = other;
		return *this;
	}

	auto operator()() { refresh(); return m_color; }

	constexpr Color getColor() const { return m_color; }
	constexpr Color& getColorRef() { return m_color; }
	constexpr bool getRainbow() const { return m_rainbow; }
	constexpr bool& getRainbowRef() { return m_rainbow; }
	constexpr float getSpeed() const { return m_speed; }
	constexpr float& getSpeedRef() { return m_speed; }
	void refresh();
private:
	void updateCol(float gameTime, float multiply);
	bool m_rainbow;
	Color m_color;
	float m_speed = 0.5f;
};