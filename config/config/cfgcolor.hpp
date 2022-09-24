#pragma once

#include <SDK/Color.hpp>

class CfgColor
{
public:
	CfgColor() = default;
	// auto updater included.
	CfgColor(const CfgColor& other);
	CfgColor(const Color& color, bool rainbow = false, float speed = 0.5f);

	CfgColor& operator=(const CfgColor& other);
	CfgColor& operator=(const Color& other);

	constexpr Color getColor() const { return m_color; }
	constexpr Color& getColorRef() { return m_color; }
	constexpr bool getRainbow() const { return m_rainbow; }
	constexpr bool& getRainbowRef() { return m_rainbow; }
	constexpr float getSpeed() const { return m_speed; }
	constexpr float& getSpeedRef() { return m_speed; }
private:
	void updateCol(float gameTime, float multiply);
	void refresh();
	bool m_rainbow;
	Color m_color;
	float m_speed = 0.5f;
};
