#pragma once

#include "../../../classes/screen2dEffects.hpp"

#include <vector>

class IMaterial;

class ScreenEffects : public Screen2DEffectsType
{
public:
	ScreenEffects()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run();
private:
	std::vector<std::pair<IMaterial*, bool>> m_materials;
};

[[maybe_unused]] inline auto g_ScreenEffects = ScreenEffects{};
