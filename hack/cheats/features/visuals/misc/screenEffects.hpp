#pragma once

#include <cheats/classes/screen2dEffects.hpp>

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
private:
	void initMaterials();

	std::vector<std::pair<IMaterial*, bool>> m_materials;
};

GLOBAL_FEATURE(ScreenEffects);
