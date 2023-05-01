#pragma once

#include <vector>

enum FrameStage;
class IConVar;

namespace skybox
{
	void reloadCustomSkyboxes();

	void run(FrameStage stage);
	void init();
	void shutdown();

	inline std::vector<std::string> customSkyboxes;

	inline bool changedState{ false };
}
