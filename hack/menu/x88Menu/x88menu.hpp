#pragma once

#include <string>

#include "x88types.hpp"
#include <cheats/classes/wndProcKeyHandler.hpp>
#include <cheats/classes/renderableToPresent.hpp>

class X88Menu : protected WndProcKeyHandler, protected RenderablePresentType
{
public:
	constexpr X88Menu() :
		WndProcKeyHandler{},
		RenderablePresentType{}
	{}

	void setStyles();
protected:
	virtual void draw() override;
	virtual void updateKeys() override;
private:
	// in pixels, padding for X
	[[nodiscard]] size_t addSpaces(const std::string& text);
	size_t index{ 0 };
	size_t m_longestNameSize{ };
	bool m_inited{ false };
	X88Types x88types{ };
};

GLOBAL_FEATURE(X88Menu);