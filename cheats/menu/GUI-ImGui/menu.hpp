#pragma once

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
private:
	bool m_active = true;
	void renderAll();
};

inline ImGuiMenu menu;