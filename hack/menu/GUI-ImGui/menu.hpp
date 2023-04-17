#pragma once

#include <cheats/classes/wndProcKeyHandler.hpp>
#include <cheats/classes/renderableToPresent.hpp>
#include <render/Color.hpp>

#include <imgui.h>
#include <string>

class ImGuiMenu final : protected WndProcKeyHandler, protected RenderablePresentType
{
public:
	constexpr ImGuiMenu() :
		WndProcKeyHandler{}
	{}

	[[nodiscard]] constexpr bool isMenuActive() const { return m_active; }
	void setStyles();	
protected:
	virtual void draw() override;
	virtual void updateKeys() override;
private:
	bool m_active{ true };
	float sharedAlpha{ 0.0f };
	std::string m_iniFile{ };
	static constexpr auto m_menuTitle{ "csgo legit" };
	
	void renderAll();
};

GLOBAL_FEATURE(ImGuiMenu);