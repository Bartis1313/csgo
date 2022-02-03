#pragma once
#include "Misc/TextureHolder/TextureHolder.hpp"
#include "mainMenu.hpp"
#include "../../../SDK/Color.hpp"
#include <memory>

namespace GUI
{
	void initGui();
	void draw();

	// this all should be stored in vars, preferably better to create config with setters or own config class to make it easier
	inline bool testBool;
	inline std::vector<std::string> m_vec = { "first", "sec", "third", "forth", "fith", "sixth" };

	inline std::vector<bool> m_vec2 = { false, false, false, false, false, false };
	inline int selected = -1;
	inline int selected2 = -1;
	inline Color colorD = Colors::Green;

	inline int key;
	inline float floating;
	inline std::string text;

	// should move it somewhere in good place
	inline TextureHolder* fatchad;

	inline Menu* menu;
}