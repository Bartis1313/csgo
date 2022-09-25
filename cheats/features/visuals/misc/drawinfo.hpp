#pragma once

#include <classes/renderableToPresent.hpp>

class Info : public RenderablePresentType
{
public:
	constexpr Info() :
		RenderablePresentType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_Information = Info{};
