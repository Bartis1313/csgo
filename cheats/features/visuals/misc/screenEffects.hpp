#pragma once

#include <classes/screen2dEffects.hpp>

#include <vector>

class IMaterial;

class ScreenEffects : protected Screen2DEffectsType
{
public:
	constexpr ScreenEffects() :
		Screen2DEffectsType{}
	{}

protected:
	virtual void run() override;
	virtual void init() override;
private:
	std::vector<std::pair<IMaterial*, bool>> m_materials;
};

GLOBAL_FEATURE(ScreenEffects);
