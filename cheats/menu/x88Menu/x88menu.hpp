#pragma once

#include <string>

#include "x88types.hpp"

class X88Menu
{
public:
	void draw();
	void init();
	void handleKeys();
private:
	// in pixels, padding for X
	[[nodiscard]] size_t addSpaces(const std::string& text);
	size_t index = 0;
	size_t m_longestNameSize;
	bool m_inited = false;
	X88Types x88types;
};

inline X88Menu x88menu;