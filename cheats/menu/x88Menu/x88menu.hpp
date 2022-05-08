#pragma once

#include <string>

// simple
class X88Menu
{
public:
	void draw();
	void init();
	void handleKeys();
private:
	// in pixels, padding for X
	size_t addSpaces(const std::string& text);
	size_t index = 0;
	size_t m_longestNameSize;
	bool m_inited = false;
};

inline X88Menu x88menu;