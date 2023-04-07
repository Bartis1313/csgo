#pragma once

#include <cheats/classes/frameStage.hpp>

#include <string>
#include <filesystem>
#include <vector>

class IConVar;

class SkyboxEdit : protected FrameStageType
{
public:
	constexpr SkyboxEdit() :
		FrameStageType{}
	{}

	void reloadCustomSkyboxes();
	std::vector<std::string> getAllCustomSkyBoxes() const { return m_allCustomSkyboxes; }
protected:
	virtual void run(int frame) override;
	virtual void init() override;
private:
	void loadSkybox(const std::string& name);
	void checkCustomSkybox();

	std::filesystem::path m_pathCustomSkybox;
	std::vector<std::string> m_allCustomSkyboxes;

	bool m_changed{ false };
	IConVar* sv_skyname{ nullptr };
public:
	void handleButtonState(bool state) { m_changed = state; }
};

GLOBAL_FEATURE(SkyboxEdit);
