#pragma once

#include <classes/renderableToSurface.hpp>
#include <classes/doPostScreen.hpp>

class IMaterial;

class NoScope : protected RenderableSurfaceType
{
public:
	constexpr NoScope() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(NoScope);

class NoScopeBlur : protected DoPostScreenType
{
public:
	constexpr NoScopeBlur() :
		DoPostScreenType{}
	{}

protected:
	virtual void run() override;
	virtual void init() override;
private:
	IMaterial* m_blurScope;
};

GLOBAL_FEATURE(NoScopeBlur);
