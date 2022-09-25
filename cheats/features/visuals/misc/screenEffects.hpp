#pragma once

#include <classes/screen2dEffects.hpp>

#include <vector>

class IMaterial;

class ScreenEffects : public Screen2DEffectsType
{
public:
	constexpr ScreenEffects() :
		Screen2DEffectsType{}
	{}

	virtual void init();
	virtual void run();
private:
	std::vector<std::pair<IMaterial*, bool>> m_materials;
};

[[maybe_unused]] inline auto g_ScreenEffects = ScreenEffects{};
