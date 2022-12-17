#pragma once

#include <SDK/Color.hpp>

class ImGuiMenu final
{
public:
	ImGuiMenu() = default;

	void init();
	void shutdown();
	void example();
	void draw();
	[[nodiscard]] constexpr bool isMenuActive() const { return m_active; }
	constexpr void changeActive() { m_active = !m_active; }
	bool inTransmission();
private:
	bool m_active = true;
	float sharedAlpha = 0.0f;
	void renderAll();
};

inline ImGuiMenu menu;