#pragma once

#include <classes/frameStage.hpp>

#include <string>
#include <filesystem>
#include <vector>

class SkyboxEdit : public FrameStageType
{
public:
	constexpr SkyboxEdit() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
	void reloadCustomSkyboxes();
	std::vector<std::string> getAllCustomSkyBoxes() const { return m_allCustomSkyboxes; }
private:
	void loadSkybox(const std::string& name);
	void checkCustomSkybox();

	std::filesystem::path m_pathCustomSkybox;
	std::vector<std::string> m_allCustomSkyboxes;
};

[[maybe_unused]] inline auto g_SkyboxEdit = SkyboxEdit{};
