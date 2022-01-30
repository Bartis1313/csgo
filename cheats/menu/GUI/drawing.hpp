#pragma once
#include "Misc/TextureHolder/TextureHolder.hpp"
#include "mainMenu.hpp"
#include <memory>

namespace GUI
{
	void initGui();
	void draw();

	// this all should be stored in vars, preferably better to create config with setters or own config class to make it easier
	inline bool testBool;
	inline std::vector<std::string> m_vec = { "first", "sec", "third", "forth", "fith", "sixth" };
	inline int selected;

	// should move it somewhere in good place
	inline TextureHolder fatchad;

	inline Menu* menu;
}