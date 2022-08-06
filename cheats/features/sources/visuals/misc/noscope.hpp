#pragma once

#include "../../../classes/renderableToSurface.hpp"
#include "../../../classes/doPostScreen.hpp"

class IMaterial;

class NoScope : public RenderableSurfaceType
{
public:
	NoScope()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_NoScope = NoScope{};

class NoScopeBlur : DoPostScreenType
{
public:
	NoScopeBlur()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run();
private:
	IMaterial* m_blurScope;
};

[[maybe_unused]] inline auto g_NoScopeBlur = NoScopeBlur{};