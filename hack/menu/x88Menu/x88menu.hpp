#pragma once

#include <string>

#include "x88types.hpp"
#include <cheats/classes/wndProcKeyHandler.hpp>

class X88Menu : protected WndProcKeyHandler
{
public:
	constexpr X88Menu() :
		WndProcKeyHandler{}
	{}

	void draw();
	void init();
protected:
	virtual void updateKeys() override;
private:
	// in pixels, padding for X
	[[nodiscard]] size_t addSpaces(const std::string& text);
	size_t index = 0;
	size_t m_longestNameSize;
	bool m_inited = false;
	X88Types x88types;
};

inline X88Menu x88menu;