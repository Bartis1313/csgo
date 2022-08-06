#pragma once

#include "../../../classes/frameStage.hpp"

#include <string>
#include <filesystem>
#include <vector>

class SkyboxEdit : public FrameStageType
{
public:
	SkyboxEdit()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(int frame);
	void reloadCustomSkyboxes();
	std::vector<std::string> getAllCustomSkyBoxes() const { return m_allCustomSkyboxes; }
private:
	void loadSkybox(const std::string& name);
	void checkCustomSkybox();

	uintptr_t m_loadSkyAddr;

	std::filesystem::path m_pathCustomSkybox;
	std::vector<std::string> m_allCustomSkyboxes;
};

[[maybe_unused]] inline auto g_SkyboxEdit = SkyboxEdit{};
