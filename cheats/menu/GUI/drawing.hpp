#pragma once
#include "Misc/TextureHolder/TextureHolder.hpp"
#include "mainMenu.hpp"
#include "../../../config/config.hpp"

namespace GUI
{
	void initGui();
	void draw();

	inline std::string configName = "";

	inline TextureHolder* fatchad;

	inline Menu* menu;
}