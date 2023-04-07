#pragma once

#include <cheats/classes/wndProcKeyHandler.hpp>
#include <render/Color.hpp>

#include <imgui.h>
#include <string>

class ImGuiMenu final : protected WndProcKeyHandler
{
public:
	constexpr ImGuiMenu() :
		WndProcKeyHandler{}
	{}

	void init();
	void shutdown();
	void example();
	void draw();
	bool inTransmission();

	[[nodiscard]] constexpr bool isMenuActive() const { return m_active; }

	struct NamedPair
	{
		ImVec2 pos;
		ImVec2 size;
	};
	std::string m_iniFile;
protected:
	virtual void updateKeys() override;
private:
	bool m_active = true;
	float sharedAlpha = 0.0f;
	static constexpr auto m_menuTitle{ "csgo legit" };
	

	ImVec2 m_windowSize;
	ImVec2 m_centrePos;
	ImVec2 m_windowPos;
	ImVec2 m_targetSize;

	NamedPair getPosAndSizeSetting(const std::string& fileName, const std::string& windowName);
	void renderAll();
};

inline ImGuiMenu menu;